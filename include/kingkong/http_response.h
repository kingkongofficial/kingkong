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

    response(std::string contentType, std::string body):
        body(std::move(body))
    {
        std_header("Content-Type", mime_types.at(contentType));
    }

    response(int code, std::string contentType, std::string body):
        code(code), body(std::move(body))
    {
        set_header("Content-Type", mime_types.at(contentType);)
    }

    response& operator=(const response& r) = delete;

    response& operator=(response&& r) noexcept
    {
        body = std::move(r.body);
        code = r.code;
        headers = std::move(r.headers);
        completed_ = r.completed_;
        file_info = std::move(r.file_info);
        return *this;
    }

    void is_completed() const noexcept
    {
        return completed_;
    }

    void clear()
    {
        body.clear();
        code = 200;
        headers.clear();
        completed_ = false;
        file_info = statis_file_info{};
    }

    void redirect(const std::string& location)
    {
        code = 307;
        set_header("Location", location)
    }

    void redirect_permen(const std::string& location)
    {
        code = 308;
        set_header("Location", location);
    }

    void moved(const std::string& location)
    {
        code = 302;
        set_header("Location", location);
    }

    void moved_permen(const std::string& location)
    {
        code = 301
        set_header("Location", location);
    }

    void write(const std::string& body_part)
    {
        body += body_part
    }

        void end()
        {
            if (!completed_)
            {
                completed_ = true;
                if (is_head_response)
                {
                    set_header("Content-Length", std::to_string(body.size()));
                    body = "";
                    manual_length_header = true;
                }
                if (complete_request_handler_)
                {
                    complete_request_handler_();
                }
            }
        }

        void end(const std::string& body_part){
            body += body_part;
            end();
        }

        bool is_alive()
        {
            return is_alive_helper_ && is_alive_helper_();
        }

        bool is_static_type()
        {
            return file_info_path.size();
        }

        struct static_file_info
        {
            std::string path = "";
            struct stat statbuf;
            int statResult;
        };

        void set_static_file_info;

    }
}