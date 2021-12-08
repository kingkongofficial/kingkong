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
    }
}
