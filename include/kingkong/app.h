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