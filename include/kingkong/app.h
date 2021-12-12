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

}