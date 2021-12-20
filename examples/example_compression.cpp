#include "kingkong.h"
#include "kingkong/compression.h"

int main()
{
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/hello")
    ([&](const kingkong::request&, kingkong::response& res) {
        res.compressed = false;

        res.body = "Hello World! This is uncompressed!";
        res.end();
    });

    KINGKONG_ROUTE(app, "/hello_compressed")
    ([]() {
        return "Hello World! This is compressed by default!";
    });


    app.port(18080)
      .use_compression(kingkong::compression::algorithm::DEFLATE)
      .loglevel(kingkong::LogLevel::Debug)
      .multithreaded()
      .run();
}