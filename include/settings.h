/* all the statics */

#pragma once

/**
 * @brief enable logging
 * 
 */
#define KINGKONG_ENABLE_LOGGING

/**
 * @brief log levels
 * 
 */
#ifndef KINGKONG_LOG_LEVEL
#define KINGKONG_LOG_LEVEL 1
#endif

/**
 * @brief static dir functions 
 */
#ifndef KINGKONG_STATIC_DIR
#define KONGKONG_STATIC_DIR "static/"
#endif 

#ifndef KINGKONG_STATIC_ENDPOINT
#define KONGKONG_STATIC_ENDPOINT "/static/<path>/"
#endif 

#if defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#define KINGKONG_CAN_USE_CPP14
#endif
#if __cplusplus >= 201402L
#define KINGKONG_CAN_USE_CPP14
#endif
