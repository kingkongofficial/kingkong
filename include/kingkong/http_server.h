#pragma once

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#ifdef KINGKONG_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#include <cstdint>
#include <atomic>
#include <future>
#include <vector>
#include <memory>

#include "kingkong/version.h"
#include "kingkong/http_connection.h"
#include "kingkong/logging.h"
#include "kingkong/task_timer.h"