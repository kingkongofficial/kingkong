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
    
    void set_header(std::string key, std::string value)
    {
        headers.erase(key);
        headers.emplace(std::move(key), std::move(value));
    }

    void add_headers(std::string key, std::string value)
    {
        headers.emplace(std::move(key), std::move(value));
    }

    const std::string& get_header_value(const std::string& key)
    {
        return kingkong::get_header_value(headers, key);
    }

    response() {}
    explicit response(int code) : code(code) {}
    response(std::string body) : body(std::move(body)) {}
    response(int code, std::string body) : code(code), body(std::move(body)) {}

    response(returnable&& value)
    {
        body = value.dump();
        set_header("Content-Type", value.content_type);
    }

    response(returnable& value)
    {
        body = value.dump();
        set_header("Content-Type", value.content_type);
    }

    response(int code, returnable& value):
        code(code)
    {
        body = value.dump();
        set_header("Content-Type", value.content_type);
    }

    response(response&& r)
    {
        *this = std::move(r);
    }

    }
}