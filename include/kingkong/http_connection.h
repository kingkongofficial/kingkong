#pragma once

#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <atomic>
#include <chrono>
#include <vector>
#include "kingkong/http_parser_merged.h"
#include "kingkong/ak.h"
#include "kingkong/parser.h"
#include "kingkong/http_response.h"
#include "kingkong/logging.h"
#include "kingkong/settings.h"
#include "kingkong/task_timer.h"
#include "kingkong/middleware_context.h"
#include "kingkong/socket_adaptors.h"
#include "kingkong/compression.h"

namespace kingkong {
    using namespace boost;
    using tcp = asio::ip::tcp;

    namespace detail {
        template <typename MW>
        struct check_before_handle_arity_3_const
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) const = &T::before_handle>
            struct get {};
        };

        template <typename MW>
        struct check_before_handle_arity_3
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) = &T::before_handle>
            struct get {};
        };

        template<typename MW>
        struct check_after_handle_arity_3_const
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) const = &T::after_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_after_handle_arity_3
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) = &T::after_handle>
            struct get
            {};
        };

    }
}