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
#include "kingkong/task_timer.h"

namespace kingkong {
    using namespace boost;
    using tcp = asio::ip::tcp;

    template <typenmae Handler, typenmae Adaptor = SocketAdaptor, typename... Middlewares>
    class Server
    {
    public:
        Server(Handler* handler, std::string bindaddr, uint16_t port, std::string server_name = std::string("Crow/") + VERSION, std::tuple<Middlewares...>* middlewares = nullptr, uint16_t concurrency = 1, uint8_t timeout = 5, typename Adaptor::context* adaptor_ctx = nullptr):
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
    }
}