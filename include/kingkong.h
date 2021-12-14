/* callback function */

/**
 * @brief this file calls all the header in the kingkong/ dir
 * 
 */

#include "kingkong/query_string.h"
#include "kingkong/http_parser_merged.h"
#include "kingkong/map.h"
#include "kingkong/TinySHA1.hpp"
#include "kingkong/settings.h"
#include "kingkong/json.h"
#include "kingkong/socket_adaptors.h"
#include "kingkong/mustache.h"
#include "kingkong/logging.h"
#include "kingkong/tasktimer.h"
#include "kingkong/utils.h"
#include "kingkong/ak.h"
#include "kingkong/http_request.h"
#include "kingkong/websocket.h"
#include "kingkong/parser.h"
#include "kingkong/http_response.h"
#include "kingkong/multipart.h"
#include "kingkong/routing.h"
#include "kingkong/middleware_context.h"
#include "kingkong/compression.h"
#include "kingkong/http_connection.h"
#include "kingkong/http_server.h"
#include "kingkong/app.h"