#pragma once

#include "kingkong/utility.h"
#include "kingkong/http_request.h"
#include "kingkong/http_response.h"

namespace kingkong {
    namespace detail
    {


        template<typename... Middlewares>
        struct partial_context : public magic::pop_back<Middlewares...>::template rebind<partial_context>, public magic::last_element_type<Middlewares...>::type::context
        {
            using parent_context = typename magic::pop_back<Middlewares...>::template rebind<::kingkong::detail::partial_context>;
            template<int N>
            using partial = typename std::conditional<N == sizeof...(Middlewares) - 1, partial_context, typename parent_context::template partial<N>>::type;

            template<typename T>
            typename T::context& get()
            {
                return static_cast<typename T::context&>(*this);
            }
        };



        template<>
        struct partial_context<>
        {
            template<int>
            using partial = partial_context;
        };



        template<int N, typename Context, typename Container, typename CurrentMW, typename... Middlewares>
        bool middleware_call_helper(Container& middlewares, request& req, response& res, Context& ctx);



        template<typename... Middlewares>
        struct context : private partial_context<Middlewares...>

        {
            template<int N, typename Context, typename Container>
            friend typename std::enable_if<(N == 0)>::type after_handlers_call_helper(Container& middlewares, Context& ctx, request& req, response& res);
            template<int N, typename Context, typename Container>
            friend typename std::enable_if<(N > 0)>::type after_handlers_call_helper(Container& middlewares, Context& ctx, request& req, response& res);

            template<int N, typename Context, typename Container, typename CurrentMW, typename... Middlewares2>
            friend bool middleware_call_helper(Container& middlewares, request& req, response& res, Context& ctx);

            template<typename T>
            typename T::context& get()
            {
                return static_cast<typename T::context&>(*this);
            }

            template<int N>
            using partial = typename partial_context<Middlewares...>::template partial<N>;
        };
    } 
} 