#pragma once

#include "kingkong/utils.h"
#include "kingkong/http_request.h"
#include "kingkong/http_response.h"

namespace kingkong {
    namespace detail {
    
        template<>
        struct partial_context<>
        {
            template<int>
            using partial = partial_context;
        };
    }
}