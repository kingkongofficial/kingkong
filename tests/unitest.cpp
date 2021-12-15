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
   TaggedRule<> r("/http/") ;
   r.name("abc");

   try 
   {
       r.validate();
       FAIL_CHECK("empty handler should fail to validate");
   }
   catch (runtime_error& e)
   {
   }

   int x = 0;

   r([&x] {
       x = 1;
       return "";
   });
   
   r.validate();

   response res;

   CHECK(0 == x);

}