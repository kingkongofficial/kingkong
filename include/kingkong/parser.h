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
    }
}