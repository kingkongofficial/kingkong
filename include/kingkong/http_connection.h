#pragma once


#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <atomic>
#include <chrono>
#include <vector>
#include "kingkong/http_parser_merged.h"
#include "kingkong/ak.h"
#include "kingkong/parser.h"
#include "kingkong/http_response.h"
#include "kingkong/logging.h"
#include "kingkong/settings.h"
#include "kingkong/task_timer.h"
#include "kingkong/middleware_context.h"
#include "kingkong/socket_adaptors.h"
#include "kingkong/compression.h"

namespace kingkong {
    using namespace boost;
    using tcp = asio::ip::tcp;

    namespace detail
    {
        template<typename MW>
        struct check_before_handle_arity_3_const
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) const = &T::before_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_before_handle_arity_3
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) = &T::before_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_after_handle_arity_3_const
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) const = &T::after_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_after_handle_arity_3
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) = &T::after_handle>
            struct get
            {};
        };

        template<typename T>
        struct is_before_handle_arity_3_impl
        {
            template<typename C>
            static std::true_type f(typename check_before_handle_arity_3_const<T>::template get<C>*);

            template<typename C>
            static std::true_type f(typename check_before_handle_arity_3<T>::template get<C>*);

            template<typename C>
            static std::false_type f(...);

        public:
            static const bool value = decltype(f<T>(nullptr))::value;
        };

        template<typename T>
        struct is_after_handle_arity_3_impl
        {
            template<typename C>
            static std::true_type f(typename check_after_handle_arity_3_const<T>::template get<C>*);

            template<typename C>
            static std::true_type f(typename check_after_handle_arity_3<T>::template get<C>*);

            template<typename C>
            static std::false_type f(...);

        public:
            static const bool value = decltype(f<T>(nullptr))::value;
        };

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<!is_before_handle_arity_3_impl<MW>::value>::type
          before_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext&)
        {
            mw.before_handle(req, res, ctx.template get<MW>(), ctx);
        }

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<is_before_handle_arity_3_impl<MW>::value>::type
          before_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext&)
        {
            mw.before_handle(req, res, ctx.template get<MW>());
        }

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<!is_after_handle_arity_3_impl<MW>::value>::type
          after_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext&)
        {
            mw.after_handle(req, res, ctx.template get<MW>(), ctx);
        }

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<is_after_handle_arity_3_impl<MW>::value>::type
          after_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext&)
        {
            mw.after_handle(req, res, ctx.template get<MW>());
        }

        template<int N, typename Context, typename Container, typename CurrentMW, typename... Middlewares>
        bool middleware_call_helper(Container& middlewares, request& req, response& res, Context& ctx)
        {
            using parent_context_t = typename Context::template partial<N - 1>;
            before_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));

            if (res.is_completed())
            {
                after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
                return true;
            }

            if (middleware_call_helper<N + 1, Context, Container, Middlewares...>(middlewares, req, res, ctx))
            {
                after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
                return true;
            }

            return false;
        }

        template<int N, typename Context, typename Container>
        bool middleware_call_helper(Container&, request&, response&, Context&)
        {
            return false;
        }

        template<int N, typename Context, typename Container>
        typename std::enable_if<(N < 0)>::type
          after_handlers_call_helper(Container&, Context&, request&, response&)
        {
        }

        template<int N, typename Context, typename Container>
        typename std::enable_if<(N == 0)>::type after_handlers_call_helper(Container& middlewares, Context& ctx, request& req, response& res)
        {
            using parent_context_t = typename Context::template partial<N - 1>;
            using CurrentMW = typename std::tuple_element<N, typename std::remove_reference<Container>::type>::type;
            after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
        }

        template<int N, typename Context, typename Container>
        typename std::enable_if<(N > 0)>::type after_handlers_call_helper(Container& middlewares, Context& ctx, request& req, response& res)
        {
            using parent_context_t = typename Context::template partial<N - 1>;
            using CurrentMW = typename std::tuple_element<N, typename std::remove_reference<Container>::type>::type;
            after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
            after_handlers_call_helper<N - 1, Context, Container>(middlewares, ctx, req, res);
        }
    }

#ifdef KINGKONG_ENABLE_DEBUG
    static std::atomic<int> connectionCount;
#endif

    template<typename Adaptor, typename Handler, typename... Middlewares>
    class Connection
    {
        friend struct kingkong::response;

    public:
        Connection(
          boost::asio::io_service& io_service,
          Handler* handler,
          const std::string& server_name,
          std::tuple<Middlewares...>* middlewares,
          std::function<std::string()>& get_cached_date_str_f,
          detail::task_timer& task_timer,
          typename Adaptor::context* adaptor_ctx_):
          adaptor_(io_service, adaptor_ctx_),
          handler_(handler),
          parser_(this),
          server_name_(server_name),
          middlewares_(middlewares),
          get_cached_date_str(get_cached_date_str_f),
          task_timer_(task_timer),
          res_stream_threshold_(handler->stream_threshold())
        {
#ifdef KINGKONG_ENABLE_DEBUG
            connectionCount++;
            KINGKONG_LOG_DEBUG << "Connection open, total " << connectionCount << ", " << this;
#endif
        }

        ~Connection()
        {
            res.complete_request_handler_ = nullptr;
            cancel_deadline_timer();
#ifdef KINGKONG_ENABLE_DEBUG
            connectionCount--;
            KINGKONG_LOG_DEBUG << "Connection closed, total " << connectionCount << ", " << this;
#endif
        }

        decltype(std::declval<Adaptor>().raw_socket())& socket()
        {
            return adaptor_.raw_socket();
        }

        void start()
        {
            adaptor_.start([this](const boost::system::error_code& ec) {
                if (!ec)
                {
                    start_deadline();

                    do_read();
                }
                else
                {
                    KINGKONG_LOG_ERROR << "Could not start adaptor: " << ec.message();
                    check_destroy();
                }
            });
        }

        void handle_header()
        {
            if (parser_.check_version(1, 1) && parser_.headers.count("expect") && get_header_value(parser_.headers, "expect") == "100-continue")
            {
                buffers_.clear();
                static std::string expect_100_continue = "HTTP/1.1 100 Continue\r\n\r\n";
                buffers_.emplace_back(expect_100_continue.data(), expect_100_continue.size());
                do_write();
            }
        }

        void handle()
        {
            cancel_deadline_timer();
            bool is_invalid_request = false;
            add_keep_alive_ = false;

            req_ = std::move(parser_.to_request());
            request& req = req_;

            req.remote_ip_address = adaptor_.remote_endpoint().address().to_string();

            if (parser_.check_version(1, 0))
            {
                if (req.headers.count("connection"))
                {
                    if (boost::iequals(req.get_header_value("connection"), "Keep-Alive"))
                        add_keep_alive_ = true;
                }
                else
                    close_connection_ = true;
            }
            else if (parser_.check_version(1, 1))
            {
                if (req.headers.count("connection"))
                {
                    if (req.get_header_value("connection") == "close")
                        close_connection_ = true;
                    else if (boost::iequals(req.get_header_value("connection"), "Keep-Alive"))
                        add_keep_alive_ = true;
                }
                if (!req.headers.count("host"))
                {
                    is_invalid_request = true;
                    res = response(400);
                }
                if (parser_.is_upgrade())
                {
                    if (req.get_header_value("upgrade") == "h2c")
                    {
                    }
                    else
                    {
                        close_connection_ = true;
                        handler_->handle_upgrade(req, res, std::move(adaptor_));
                        return;
                    }
                }
            }

            KINGKONG_LOG_INFO << "Request: " << boost::lexical_cast<std::string>(adaptor_.remote_endpoint()) << " " << this << " HTTP/" << parser_.http_major << "." << parser_.http_minor << ' '
                          << method_name(req.method) << " " << req.url;


            need_to_call_after_handlers_ = false;
            if (!is_invalid_request)
            {
                res.complete_request_handler_ = [] {};
                res.is_alive_helper_ = [this]() -> bool {
                    return adaptor_.is_open();
                };

                ctx_ = detail::context<Middlewares...>();
                req.middleware_context = static_cast<void*>(&ctx_);
                req.io_service = &adaptor_.get_io_service();
                detail::middleware_call_helper<0, decltype(ctx_), decltype(*middlewares_), Middlewares...>(*middlewares_, req, res, ctx_);

                if (!res.completed_)
                {
                    res.complete_request_handler_ = [this] {
                        this->complete_request();
                    };
                    need_to_call_after_handlers_ = true;
                    handler_->handle(req, res);
                    if (add_keep_alive_)
                        res.set_header("connection", "Keep-Alive");
                }
                else
                {
                    complete_request();
                }
            }
            else
            {
                complete_request();
            }
        }

        void complete_request()
        {
            KINGKONG_LOG_INFO << "Response: " << this << ' ' << req_.raw_url << ' ' << res.code << ' ' << close_connection_;

            if (need_to_call_after_handlers_)
            {
                need_to_call_after_handlers_ = false;

                detail::after_handlers_call_helper<
                  (static_cast<int>(sizeof...(Middlewares)) - 1),
                  decltype(ctx_),
                  decltype(*middlewares_)>(*middlewares_, ctx_, req_, res);
            }
#ifdef KINGKONG_ENABLE_COMPRESSION
            if (handler_->compression_used())
            {
                std::string accept_encoding = req_.get_header_value("Accept-Encoding");
                if (!accept_encoding.empty() && res.compressed)
                {
                    switch (handler_->compression_algorithm())
                    {
                        case compression::DEFLATE:
                            if (accept_encoding.find("deflate") != std::string::npos)
                            {
                                res.body = compression::compress_string(res.body, compression::algorithm::DEFLATE);
                                res.set_header("Content-Encoding", "deflate");
                            }
                            break;
                        case compression::GZIP:
                            if (accept_encoding.find("gzip") != std::string::npos)
                            {
                                res.body = compression::compress_string(res.body, compression::algorithm::GZIP);
                                res.set_header("Content-Encoding", "gzip");
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
#endif
            std::string location = res.get_header_value("Location");
            if (!location.empty() && location.find("://", 0) == std::string::npos)
            {
#ifdef KINGKONG_ENABLE_SSL
                if (handler_->ssl_used())
                    location.insert(0, "https://" + req_.get_header_value("Host"));
                else
#endif
                    location.insert(0, "http://" + req_.get_header_value("Host"));
                res.set_header("location", location);
            }

            prepare_buffers();

            if (res.is_static_type())
            {
                do_write_static();
            }
            else
            {
                do_write_general();
            }
        }

    private:
        void prepare_buffers()
        {
            res.complete_request_handler_ = nullptr;

            if (!adaptor_.is_open())
            {
                return;
            }

            static std::unordered_map<int, std::string> statusCodes = {
              {status::CONTINUE, "HTTP/1.1 100 Continue\r\n"},
              {status::SWITCHING_PROTOCOLS, "HTTP/1.1 101 Switching Protocols\r\n"},

              {status::OK, "HTTP/1.1 200 OK\r\n"},
              {status::CREATED, "HTTP/1.1 201 Created\r\n"},
              {status::ACCEPTED, "HTTP/1.1 202 Accepted\r\n"},
              {status::NON_AUTHORITATIVE_INFORMATION, "HTTP/1.1 203 Non-Authoritative Information\r\n"},
              {status::NO_CONTENT, "HTTP/1.1 204 No Content\r\n"},
              {status::RESET_CONTENT, "HTTP/1.1 205 Reset Content\r\n"},
              {status::PARTIAL_CONTENT, "HTTP/1.1 206 Partial Content\r\n"},

              {status::MULTIPLE_CHOICES, "HTTP/1.1 300 Multiple Choices\r\n"},
              {status::MOVED_PERMANENTLY, "HTTP/1.1 301 Moved Permanently\r\n"},
              {status::FOUND, "HTTP/1.1 302 Found\r\n"},
              {status::SEE_OTHER, "HTTP/1.1 303 See Other\r\n"},
              {status::NOT_MODIFIED, "HTTP/1.1 304 Not Modified\r\n"},
              {status::TEMPORARY_REDIRECT, "HTTP/1.1 307 Temporary Redirect\r\n"},
              {status::PERMANENT_REDIRECT, "HTTP/1.1 308 Permanent Redirect\r\n"},

              {status::BAD_REQUEST, "HTTP/1.1 400 Bad Request\r\n"},
              {status::UNAUTHORIZED, "HTTP/1.1 401 Unauthorized\r\n"},
              {status::FORBIDDEN, "HTTP/1.1 403 Forbidden\r\n"},
              {status::NOT_FOUND, "HTTP/1.1 404 Not Found\r\n"},
              {status::METHOD_NOT_ALLOWED, "HTTP/1.1 405 Method Not Allowed\r\n"},
              {status::PROXY_AUTHENTICATION_REQUIRED, "HTTP/1.1 407 Proxy Authentication Required\r\n"},
              {status::CONFLICT, "HTTP/1.1 409 Conflict\r\n"},
              {status::GONE, "HTTP/1.1 410 Gone\r\n"},
              {status::PAYLOAD_TOO_LARGE, "HTTP/1.1 413 Payload Too Large\r\n"},
              {status::UNSUPPORTED_MEDIA_TYPE, "HTTP/1.1 415 Unsupported Media Type\r\n"},
              {status::RANGE_NOT_SATISFIABLE, "HTTP/1.1 416 Range Not Satisfiable\r\n"},
              {status::EXPECTATION_FAILED, "HTTP/1.1 417 Expectation Failed\r\n"},
              {status::PRECONDITION_REQUIRED, "HTTP/1.1 428 Precondition Required\r\n"},
              {status::TOO_MANY_REQUESTS, "HTTP/1.1 429 Too Many Requests\r\n"},
              {status::UNAVAILABLE_FOR_LEGAL_REASONS, "HTTP/1.1 451 Unavailable For Legal Reasons\r\n"},

              {status::INTERNAL_SERVER_ERROR, "HTTP/1.1 500 Internal Server Error\r\n"},
              {status::NOT_IMPLEMENTED, "HTTP/1.1 501 Not Implemented\r\n"},
              {status::BAD_GATEWAY, "HTTP/1.1 502 Bad Gateway\r\n"},
              {status::SERVICE_UNAVAILABLE, "HTTP/1.1 503 Service Unavailable\r\n"},
              {status::VARIANT_ALSO_NEGOTIATES, "HTTP/1.1 506 Variant Also Negotiates\r\n"},
            };

            static std::string seperator = ": ";
            static std::string crlf = "\r\n";

            buffers_.clear();
            buffers_.reserve(4 * (res.headers.size() + 5) + 3);

            if (!statusCodes.count(res.code))
                res.code = 500;
            {
                auto& status = statusCodes.find(res.code)->second;
                buffers_.emplace_back(status.data(), status.size());
            }

            if (res.code >= 400 && res.body.empty())
                res.body = statusCodes[res.code].substr(9);

            for (auto& kv : res.headers)
            {
                buffers_.emplace_back(kv.first.data(), kv.first.size());
                buffers_.emplace_back(seperator.data(), seperator.size());
                buffers_.emplace_back(kv.second.data(), kv.second.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }

            if (!res.manual_length_header && !res.headers.count("content-length"))
            {
                content_length_ = std::to_string(res.body.size());
                static std::string content_length_tag = "Content-Length: ";
                buffers_.emplace_back(content_length_tag.data(), content_length_tag.size());
                buffers_.emplace_back(content_length_.data(), content_length_.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }
            if (!res.headers.count("server"))
            {
                static std::string server_tag = "Server: ";
                buffers_.emplace_back(server_tag.data(), server_tag.size());
                buffers_.emplace_back(server_name_.data(), server_name_.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }
            if (!res.headers.count("date"))
            {
                static std::string date_tag = "Date: ";
                date_str_ = get_cached_date_str();
                buffers_.emplace_back(date_tag.data(), date_tag.size());
                buffers_.emplace_back(date_str_.data(), date_str_.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }
            if (add_keep_alive_)
            {
                static std::string keep_alive_tag = "Connection: Keep-Alive";
                buffers_.emplace_back(keep_alive_tag.data(), keep_alive_tag.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }

            buffers_.emplace_back(crlf.data(), crlf.size());
        }

        void do_write_static()
        {
            is_writing = true;
            boost::asio::write(adaptor_.socket(), buffers_);

            if (res.file_info.statResult == 0)
            {
                std::ifstream is(res.file_info.path.c_str(), std::ios::in | std::ios::binary);
                char buf[16384];
                while (is.read(buf, sizeof(buf)).gcount() > 0)
                {
                    std::vector<asio::const_buffer> buffers;
                    buffers.push_back(boost::asio::buffer(buf));
                    do_write_sync(buffers);
                }
            }

            res.end();
            res.clear();
            buffers_.clear();
        }

        void do_write_general()
        {
            if (res.body.length() < res_stream_threshold_)
            {
                res_body_copy_.swap(res.body);
                buffers_.emplace_back(res_body_copy_.data(), res_body_copy_.size());

                do_write();

                if (need_to_start_read_after_complete_)
                {
                    need_to_start_read_after_complete_ = false;
                    start_deadline();
                    do_read();
                }
            }
            else
            {
                is_writing = true;
                boost::asio::write(adaptor_.socket(), buffers_); 
                if (res.body.length() > 0)
                {
                    std::string buf;
                    std::vector<asio::const_buffer> buffers;

                    while (res.body.length() > 16384)
                    {
                        buf = res.body.substr(0, 16384);
                        res.body = res.body.substr(16384);
                        buffers.clear();
                        buffers.push_back(boost::asio::buffer(buf));
                        do_write_sync(buffers);
                    }
                    buf = res.body;
                    res.body.clear();

                    buffers.clear();
                    buffers.push_back(boost::asio::buffer(buf));
                    do_write_sync(buffers);
                }

                res.end();
                res.clear();
                buffers_.clear();
            }
        }

        void do_read()
        {
            is_reading = true;
            adaptor_.socket().async_read_some(
              boost::asio::buffer(buffer_),
              [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                  bool error_while_reading = true;
                  if (!ec)
                  {
                      bool ret = parser_.feed(buffer_.data(), bytes_transferred);
                      if (ret && adaptor_.is_open())
                      {
                          error_while_reading = false;
                      }
                  }

                  if (error_while_reading)
                  {
                      cancel_deadline_timer();
                      parser_.done();
                      adaptor_.shutdown_read();
                      adaptor_.close();
                      is_reading = false;
                      KINGKONG_LOG_DEBUG << this << " from read(1)";
                      check_destroy();
                  }
                  else if (close_connection_)
                  {
                      cancel_deadline_timer();
                      parser_.done();
                      is_reading = false;
                      check_destroy();
                  }
                  else if (!need_to_call_after_handlers_)
                  {
                      start_deadline();
                      do_read();
                  }
                  else
                  {
                      need_to_start_read_after_complete_ = true;
                  }
              });
        }

        void do_write()
        {
            is_writing = true;
            boost::asio::async_write(
              adaptor_.socket(), buffers_,
              [&](const boost::system::error_code& ec, std::size_t) {
                  is_writing = false;
                  res.clear();
                  res_body_copy_.clear();
                  if (!ec)
                  {
                      if (close_connection_)
                      {
                          adaptor_.shutdown_write();
                          adaptor_.close();
                          KINGKONG_LOG_DEBUG << this << " from write(1)";
                          check_destroy();
                      }
                  }
                  else
                  {
                      KINGKONG_LOG_DEBUG << this << " from write(2)";
                      check_destroy();
                  }
              });
        }

        inline void do_write_sync(std::vector<asio::const_buffer>& buffers)
        {

            boost::asio::write(adaptor_.socket(), buffers, [&](std::error_code ec, std::size_t) {
                if (!ec)
                {
                    if (close_connection_)
                    {
                        adaptor_.shutdown_write();
                        adaptor_.close();
                        KINGKONG_LOG_DEBUG << this << " from write (sync)(1)";
                        check_destroy();
                    }
                    return false;
                }
                else
                {
                    KINGKONG_LOG_ERROR << ec << " - happened while sending buffers";
                    KINGKONG_LOG_DEBUG << this << " from write (sync)(2)";
                    check_destroy();
                    return true;
                }
            });
        }

        void check_destroy()
        {
            KINGKONG_LOG_DEBUG << this << " is_reading " << is_reading << " is_writing " << is_writing;
            if (!is_reading && !is_writing)
            {
                KINGKONG_LOG_DEBUG << this << " delete (idle) ";
                delete this;
            }
        }

        void cancel_deadline_timer()
        {
            KINGKONG_LOG_DEBUG << this << " timer cancelled: " << &task_timer_ << ' ' << task_id_;
            task_timer_.cancel(task_id_);
        }

        void start_deadline()
        {
            cancel_deadline_timer();

            task_id_ = task_timer_.schedule([this] {
                if (!adaptor_.is_open())
                {
                    return;
                }
                adaptor_.shutdown_readwrite();
                adaptor_.close();
            });
            KINGKONG_LOG_DEBUG << this << " timer added: " << &task_timer_ << ' ' << task_id_;
        }

    private:
        Adaptor adaptor_;
        Handler* handler_;

        boost::array<char, 4096> buffer_;

        HTTPParser<Connection> parser_;
        request req_;
        response res;

        bool close_connection_ = false;

        const std::string& server_name_;
        std::vector<boost::asio::const_buffer> buffers_;

        std::string content_length_;
        std::string date_str_;
        std::string res_body_copy_;

        detail::task_timer::identifier_type task_id_;

        bool is_reading{};
        bool is_writing{};
        bool need_to_call_after_handlers_{};
        bool need_to_start_read_after_complete_{};
        bool add_keep_alive_{};

        std::tuple<Middlewares...>* middlewares_;
        detail::context<Middlewares...> ctx_;

        std::function<std::string()>& get_cached_date_str;
        detail::task_timer& task_timer_;

        size_t res_stream_threshold_;
    };

}