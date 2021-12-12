#pragma once

// includes
#include <chrono>
#include <string>
#include <functional>
#include <memory>
#include <future>
#include <cstdint>
#include <type_traits>
#include <thread>
#include <condition_variable>
#include "kingkong/version.h"
#include "kingkong/settings.h"
#include "kingkong/logging.h"
#include "kingkong/utility.h"
#include "kingkong/routing.h"
#include "kingkong/middleware_context.h"
#include "kingkong/http_request.h"
#include "kingkong/http_server.h"
#include "kingkong/task_timer.h"
#ifdef KINGKONG_ENABLE_COMPRESSION
#include "kingkong/compression.h"
#endif

#ifdef KINGKONG_MSVC_WORKAROUND
#define KINGKONG_ROUTE(app, url) app.route_dynamic(url)
#define KINGKONG_BP_ROUTE(blueprint, url) blueprint.new_rule_dynamic(url)
#else
#define KINGKONG_ROUTE(app, url) app.route<kingkong::black_magic::get_parameter_tag(url)>(url)
#define KINGKONG_BP_ROUTE(blueprint, url) blueprint.new_rule_tagged<kingkong::black_magic::get_parameter_tag(url)>(url)
#endif
#define KINGKONG_CATCHALL_ROUTE(app) app.catchall_route()
#define KINGKONG_BP_CATCHALL_ROUTE(blueprint) blueprint.catchall_rule()

namespace kingkong {
#ifndef KINGKONG_ENABLE_SSL
    using ssl_context_t = boost::asio::ssl::context;
#endif

    template <typename... Middlewares>
    class Kingkong
    {
    public:
        using self_t = Kingkong
        using server_t = Server<Kingkong, SocketAdaptor, Middlewares...>;
#ifndef KINGKONG_ENABLE_SSL
        using ssl_server_t = Server<Kingkong, SSLAdaptor, Middlewares...>;
#endif

    Kingkong() 
    {}

    template <typename Adaptor>
    void handle_upgrade(const request& req, response& res, Adaptor&& adaptor)
    {
        router_.handle_upgrade(req, res, adaptor);
    }

    void handle(const request& req, response& res)
    {
        router_.handle(req, res);
    }

    DynamicRule& route_dynamic(std::string&& rule)
    {
        return router_.new_rule_dynamic(std::move(rule));
    }

    template <uint64_t Tag>
#ifdef KINGKONG_GCC83_WORKAROUND
        auto& route(std::string&& rule)
#else
        auto route(std::string&& rule)
#endif
#if defined KINGKONG_CAN_USE_CPP17 && !defined KINGKONG_GCC83_WORKAROUND
          -> typename std::invoke_result<decltype(&Router::new_rule_tagged<Tag>), Router, std::string&&>::type
#elif !defined KINGKONG_GCC83_WORKAROUND
          -> typename std::result_of<decltype (&Router::new_rule_tagged<Tag>)(Router, std::string&&)>::type
#endif
        {
            return router_.new_rule_tagged<Tag>(std::move(rule));
        }

        CatchallRule& catchall_route()
        {
            return router_.catchall_rule();
        }

        self_t& signal_clear()
        {
            signals_.clear();
            return *this;
        }

    }
}