#define CATCH_CONIG_MAIN
#define KINGKONG_ENABLE_DEBUG
#define KINGKONG_LOG_LEVEL 0

#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "catch.hpp"
#include "kingkong.h"
#include "kingkong/middlewares/cookie_parser.h"

using namespace std;
using namespace kingkong;

#define LOCALHOST_ADDRESS "127.0.0.1"

TEST_CASE("Rule")
{
    
}