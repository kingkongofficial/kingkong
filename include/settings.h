/* all the statics */

#pragma once

/**
 * @brief enable logging
 * 
 */
#define KINGKONG_ENABLE_LOG

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