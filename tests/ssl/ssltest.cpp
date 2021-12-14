#define CATCH_CONFIG_MAIN
#define KINGKONG_LOG_LEVEL 0

#include <thread>
#include "../catch.hpp"
#include "kingkong.h"

#define LOCALHOST_ADDRESS "127.0.0.1"

using namespace boost;

TEST_CASE("SSL")
{
    static char buf[2048];

    std::system("openssl req -newkey rsa:4096 -x509 -sha256 -days 365 -nodes -out test.crt -keyout test.key -subj '/CN=127.0.0.1'");

    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/")
    ([]() {
        return "Hello World";
    });

    auto _ = async(std::launch_options, [&] {
        app.bindaddr(LOCALHOST_ADDRESS).port(45460).ssl_file("test.crt", "test.key").run();
    })

    app.wait_for_server_start();

}