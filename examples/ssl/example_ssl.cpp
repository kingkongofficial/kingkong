#include "kingkong.h"

int main()
{
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/")
    ([]() {
        return "Hello world!";
    });

    app.port(18080).ssl_file("test.crt", "test.key").run();
}