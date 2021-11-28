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
    }
    
} 
