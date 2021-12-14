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
        return "Hello world, I'm keycrt.";
    });

    auto _ = async(std::launch::async, [&] {
        app.bindaddr(LOCALHOST_ADDRESS).port(45460).ssl_file("test.crt", "test.key").run();
    });

    app.wait_for_server_start();

    std::string sendmsg = "GET /\r\n\r\n";

    asio::ssl::context ctx(asio::ssl::context::sslv23);

    asio::io_service is;
    {
        asio::ssl::stream<asio::ip::tcp::socket> c(is, ctx);
        c.lowest_layer().connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(LOCALHOST_ADDRESS), 45460));

        c.handshake(asio::ssl::stream_base::client);
        c.write_some(asio::buffer(sendmsg));

        size_t x = 0;
        size_t y = 0;

        while (x < 121)
        {
            y = c.read_some(asio::buffer(buf, 2048));
            x += y;
            buf[y] = '\0';
        }

        CHECK(std::string("Hello world, I'm keycrt.") == std::string(buf));
    }

    app.stop();

    std::system("rm test.crt test.key");
}