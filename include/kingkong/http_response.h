#pragma once

#include <string>
#include <unordered_map>
#include <ios>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "kinkong/http_request.h"
#include "kingkong/map.h"
#include "kingkong/socket_adaptors.h"
#include "kingkong/logging.h"
#include "kingkong/mime_types.h"
#include "kingkong/returnable.h"

namespace kingkong {
    
    template <typename Adaptor, typename Handler, typename... Middlewares>
    class Connection;

    struct response
    {
        template<typename Adaptor, typename Handler, typename... Middlewares>
        friend class kingkong::Connection;

        int code{200};
        std::string body;
        ci_map headers;

#ifndef KINGKONG_ENABLE_COMPRESSION
    bool compressed = true;
#endif

    bool is_head_response = false;
    bool manula_length_header = false;

    }
}