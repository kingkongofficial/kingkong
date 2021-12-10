#pragma once

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#ifdef KINGKONG_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#include <cstdint>
#include <atomic>
#include <future>
#include <vector>
#include <memory>
#include "kingkong/version.h"
#include "kingkong/http_connection.h"
#include "kingkong/logging.h"
#include "kingkong/tasktimer.h"

namespace kingkong {
    using namespace boost;
    using tcp = asio::ip::tcp;

    template<typename Handler, typename Adaptor = SocketAdaptor, typename... Middlewares>
    class Server
    {
    public:
        Server(Handler* handler, std::string bindaddr, uint16_t port, std::string server_name = std::string("Kingkong/") + VERSION, std::tuple<Middlewares...>* middlewares = nullptr, uint16_t concurrency = 1, uint8_t timeout = 5, typename Adaptor::context* adaptor_ctx = nullptr):
          acceptor_(io_service_, tcp::endpoint(boost::asio::ip::address::from_string(bindaddr), port)),
          signals_(io_service_, SIGINT, SIGTERM),
          tick_timer_(io_service_),
          handler_(handler),
          concurrency_(concurrency == 0 ? 1 : concurrency),
          timeout_(timeout),
          server_name_(server_name),
          port_(port),
          bindaddr_(bindaddr),
          task_queue_length_pool_(concurrency_),
          middlewares_(middlewares),
          adaptor_ctx_(adaptor_ctx)
        {}

        void set_tick_function(std::chrono::milliseconds d, std::function<void()> f)
        {
            tick_interval_ = d;
            tick_function_ = f;
        }

        void on_tick()
        {
            tick_function_();
            tick_timer_.expires_from_now(boost::posix_time::milliseconds(tick_interval_.count()));
            tick_timer_.async_wait([this](const boost::system::error_code& ec) {
                if (ec)
                    return;
                on_tick();
            });
        }

        void run()
        {
            for (int i = 0; i < concurrency_; i++)
                io_service_pool_.emplace_back(new boost::asio::io_service());
            get_cached_date_str_pool_.resize(concurrency_);
            task_timer_pool_.resize(concurrency_);

            std::vector<std::future<void>> v;
            std::atomic<int> init_count(0);
            for (uint16_t i = 0; i < concurrency_; i++)
                v.push_back(
                  std::async(
                    std::launch::async, [this, i, &init_count] {
                        auto last = std::chrono::steady_clock::now();

                        std::string date_str;
                        auto update_date_str = [&] {
                            auto last_time_t = time(0);
                            tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
                            gmtime_s(&my_tm, &last_time_t);
#else
                            gmtime_r(&last_time_t, &my_tm);
#endif
                            date_str.resize(100);
                            size_t date_str_sz = strftime(&date_str[0], 99, "%a, %d %b %Y %H:%M:%S GMT", &my_tm);
                            date_str.resize(date_str_sz);
                        };
                        update_date_str();
                        get_cached_date_str_pool_[i] = [&]() -> std::string {
                            if (std::chrono::steady_clock::now() - last >= std::chrono::seconds(1))
                            {
                                last = std::chrono::steady_clock::now();
                                update_date_str();
                            }
                            return date_str;
                        };

                        detail::task_timer task_timer(*io_service_pool_[i]);
                        task_timer.set_default_timeout(timeout_);
                        task_timer_pool_[i] = &task_timer;
                        task_queue_length_pool_[i] = 0;

                        init_count++;
                        while (1)
                        {
                            try
                            {
                                if (io_service_pool_[i]->run() == 0)
                                {
                                    break;
                                }
                            }
                            catch (std::exception& e)
                            {
                                KINGKONG_LOG_ERROR << "Worker Crash: An uncaught exception occurred: " << e.what();
                            }
                        }
                    }));

            if (tick_function_ && tick_interval_.count() > 0)
            {
                tick_timer_.expires_from_now(boost::posix_time::milliseconds(tick_interval_.count()));
                tick_timer_.async_wait(
                  [this](const boost::system::error_code& ec) {
                      if (ec)
                          return;
                      on_tick();
                  });
            }

            port_ = acceptor_.local_endpoint().port();
            handler_->port(port_);


            KINGKONG_LOG_INFO << server_name_ << " server is running at " << (handler_->ssl_used() ? "https://" : "http://") << bindaddr_ << ":" << acceptor_.local_endpoint().port()
                          << " using " << concurrency_ << " threads";
            KINGKONG_LOG_INFO << "Call `app.loglevel(kingkong::LogLevel::Warning)` to hide Info level logs.";

            signals_.async_wait(
              [&](const boost::system::error_code&, int) {
                  stop();
              });

            while (concurrency_ != init_count)
                std::this_thread::yield();

            do_accept();

            std::thread(
              [this] {
                  io_service_.run();
                  KINGKONG_LOG_INFO << "Exiting.";
              })
              .join();
        }

        void stop()
        {
            io_service_.stop();
            for (auto& io_service : io_service_pool_)
                io_service->stop();
        }

        void signal_clear()
        {
            signals_.clear();
        }

        void signal_add(int signal_number)
        {
            signals_.add(signal_number);
        }

    private:
        uint16_t pick_io_service_idx()
        {
            uint16_t min_queue_idx = 0;

            for (uint16_t i = 1; i < task_queue_length_pool_.size() && task_queue_length_pool_[min_queue_idx] > 0; i++)
            {
                if (task_queue_length_pool_[i] < task_queue_length_pool_[min_queue_idx])
                    min_queue_idx = i;
            }
            return min_queue_idx;
        }

        void do_accept()
        {
            uint16_t service_idx = pick_io_service_idx();
            asio::io_service& is = *io_service_pool_[service_idx];
            task_queue_length_pool_[service_idx]++;
            KINGKONG_LOG_DEBUG << &is << " {" << service_idx << "} queue length: " << task_queue_length_pool_[service_idx];

            auto p = new Connection<Adaptor, Handler, Middlewares...>(
              is, handler_, server_name_, middlewares_,
              get_cached_date_str_pool_[service_idx], *task_timer_pool_[service_idx], adaptor_ctx_, task_queue_length_pool_[service_idx]);

            acceptor_.async_accept(
              p->socket(),
              [this, p, &is, service_idx](boost::system::error_code ec) {
                  if (!ec)
                  {
                      is.post(
                        [p] {
                            p->start();
                        });
                  }
                  else
                  {
                      task_queue_length_pool_[service_idx]--;
                      KINGKONG_LOG_DEBUG << &is << " {" << service_idx << "} queue length: " << task_queue_length_pool_[service_idx];
                      delete p;
                  }
                  do_accept();
              });
        }

    private:
        asio::io_service io_service_;
        std::vector<std::unique_ptr<asio::io_service>> io_service_pool_;
        std::vector<detail::task_timer*> task_timer_pool_;
        std::vector<std::function<std::string()>> get_cached_date_str_pool_;
        tcp::acceptor acceptor_;
        boost::asio::signal_set signals_;
        boost::asio::deadline_timer tick_timer_;

        Handler* handler_;
        uint16_t concurrency_{1};
        std::uint8_t timeout_;
        std::string server_name_;
        uint16_t port_;
        std::string bindaddr_;
        std::vector<std::atomic<unsigned int>> task_queue_length_pool_;

        std::chrono::milliseconds tick_interval_;
        std::function<void()> tick_function_;

        std::tuple<Middlewares...>* middlewares_;

        typename Adaptor::context* adaptor_ctx_;
    };
} 