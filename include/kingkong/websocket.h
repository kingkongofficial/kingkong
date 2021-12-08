#pragma once

#include <boost/algorithm/string/predicate.hpp>
#include <boost/array.hpp>
#include "kingkong/socket_adaptors.h"
#include "kingkong/http_request.h"
#include "kingkong/TinySHA1.hpp"

namespace kingkong {

    namespace websocket {
        enum class WebSocketReadState
        {
            MiniHeader,
            Len16,
            Len64,
            Mask,
            Payload,
        };

        struct connection
        {
            virtual void send_binary(const std::string& msg) = 0;
            virtual void send_text(const std::string& msg) = 0;
            virtual void send_ping(const std::string& msg) = 0;
            virtual void send_pong(const std::string& msg) = 0;
            virtual void close(const std::string& msg = "quit") = 0;
            virtual std::string get_remote_ip() = 0;
            virtual ~connection() {}

            void userdata(void* u) { userdata_ = u; }
            void* userdata() { return userdata_; }

        private:
            void* userdata_;
        };
    }
}
