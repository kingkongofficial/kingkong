#pragma once

#define KINGKONG_ENABLE_LOGGING

#ifndef KINGKONG_LOG_LEVEL
#define KINGKONG_LOG_LEVEL 1
#endif

#ifndef KINGKONG_STATIC_DIRECTORY
#define KINGKONG_STATIC_DIRECTORY "static/"
#endif
#ifndef KINGKONG_STATIC_ENDPOINT
#define KINGKONG_STATIC_ENDPOINT "/static/<path>"
#endif

#if defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#define KINGKONG_CAN_USE_CPP14
#endif
#if __cplusplus >= 201402L
#define KINGKONG_CAN_USE_CPP14
#endif

#if defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#define KINGKONG_CAN_USE_CPP17
#endif
#if __cplusplus >= 201703L
#define KINGKONG_CAN_USE_CPP17
#endif

#if defined(_MSC_VER)
#if _MSC_VER < 1900
#define KINGKONG_MSVC_WORKAROUND
#define constexpr const
#define noexcept throw()
#endif
#endif