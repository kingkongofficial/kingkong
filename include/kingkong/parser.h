#pragma once

#include <string>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "kingkong/http_parser_merged.h"
#include "kingkong/http_request.h"

namespace kingkong {
    template <typename Handler>
    struct HTTPParser : public http_parser 
    {
        static int on_message_begin(http_parser* self_)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            self->clear();
            return 0;
        }

        static int on_url(http_parser* self_, const char *at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            self->raw_url.insert(self->raw_url.end(), at, at + length);
            return 0;
        }

        static int on_header_field(http_parser* self_, const char* at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            switch (self->header_building_state)
            {
                case 0:
                    if (!self->header_value.empty())
                    {
                        self->headers.emplace(std::move(self->header_field), std::move(self->header_value));
                    }
                    self->header_field.assign(at, at + length);
                    self->header_building_state = 1;
                    break;
                case 1:
                    self->header_field.insert(self->header_field.end(), at, at + length);
                    break;
            }
            return 0;
        }
    }

    static int on_header_value(http_parser* self_, const char* at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            switch (self->header_building_state)
            {
                case 0:
                    self->header_value.insert(self->header_value.end(), at, at + length);
                    break;
                case 1:
                    self->header_building_state = 0;
                    self->header_value.assign(at, at + length);
                    break;
            }
            return 0;
        }

    HTTPParser(Handler* handler):
        handler_(handler)
    {
        http_parser_init(this, HTTP_REQUEST);
    }
}