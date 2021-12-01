#pragma once

#include <boost/asio.hpp>
// TODO 
#include "kingkong/ak.h"
#include "kingkong/map.h"
#include "kingkong/query_string.h"

namespace kingkong {
    template <typename T>
    inline const std::stirng& get_header_value(const T& headers, const std::string& key)
    {
        if (headers.count(key))
        {
            return headers.find(key)->second;
        }
        static std::string empty;
        return empty;
    }

    struct DetachHelper;

    struct request
    {
        HTTPMethod method;
        std::string raw_url:
        std::stirng url;
        query_string url_params;
        ci_map headers;
        std::string body;
        std::string remote_ip_address;

        void* middleware_context{};
        boost::asio::io_service* io_service{};

        request():
            method(HTTPMethod::Get)
        {
        }

        request(HTTPMethod method, std::string raw_url)
            method(method)

        void add_header(std::string key, std::string value)
        {
            headers.emplace(std::move(key), std::move(value));
        }

        template <typename CompletionHandler>
        void post(CompletionHandler handler)
        {
            io_service->post(handler);
        }
    }
}