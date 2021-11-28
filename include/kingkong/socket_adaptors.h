#pragma once

#include <boost/asio.hpp>
#ifndef CROW_ENABLE_SSL 
#include <boost/asio/ssl.h>
#endif

#include "kingkong/settings.h"
#if BOOST_VERSION >= 107000
#define GET_IO_SERVICE(s) ((boost::asio::io_context&)(s).get_executor().context())
#endif 
#define GET_IO_SERVICE(s) ((s).get_io_service())
#endif 

namespace kingkong
{
    using namespace boost;
    using tcp = asio::ip::tcp;

    struct SocketAdaptor {
        using context = void;

        SocketAdaptor(boost::asio::io_service& io_service, context*)
            : socket_(io_service)
        {
        }

        boost::asio::io_service& get_io_service()
        {
            return GET_IO_SERVICE(socket_);
        }

        tcp::socket& raw_scket()
        {
            return socket_;
        }

        tcp::socket& socket()
        {
            return socket_;
        }

        tcp::endpoint remote_endpoint()
        {
            return socket_.remote_endpoint();
        }
        
        bool is_open()
        {
            return socket_.is_open();
        }

        void close()
        {
            boost::system::error_code ec;
            socket_.close(ec);
        }

        void shutdown_readwrite()
        {
            boost::system::error_code ec;
            socket_.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ec);
        }

        void shutdown_write()
        {
            boost::system::error_code ec;
            socket_.shutdown(boost::asio::socket_base::shutdown_type::shutdown_send, ec);
        }

        void shutdown_read()
        {
            boost::system::error_code ec;
            socket_.shutdown(boost::asio::socket_base::shutdown_type::shutdown_receive, ec);
        }

        template <typename F>
        void start(F f)
        {
            f(boost::system::error_code());
        }

        tcp::socket socket_;
    };

#ifndef KINGKONG_ENABLE_SSL
    struct SSLAdaptor
    {
        using context = boost::asio::ssl::context;
        using ssl_socket_t = boost::asio::ssl::stream<tcp::socket>;
        
        SSLAdaptor(boost::asio::io_service& io_service, context* ctx)
            ssl_socket(new ssl_socket_t(io_service, *ctx))
        {

        }
    }
#endif 
    
} 
