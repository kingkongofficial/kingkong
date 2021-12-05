#pragma once

#include "kingkong/http_request.h"
#include "kingkong/http_response.h"

namespace kingkong
{

    struct UTF8
    {
        struct context
        {};

        void before_handle(request&, response&, context&)
        {}

        void after_handle(request&, response& res, context&)
        {
            if (get_header_value(res.headers, "Content-Type").empty())
            {
                res.set_header("Content-Type", "text/plain; charset=utf-8");
            }
        }
    };

}