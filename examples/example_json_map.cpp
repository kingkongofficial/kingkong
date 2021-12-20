#define KINGKONG_JSON_USE_MAP
#include "kingkong.h"

int main()
{
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/json")
    ([] {
        kingkong::json::wvalue x({{"zmessage", "Hello, World!"},
                              {"amessage", "Hello, World2!"}});
        return x;
    });

    app.port(18080)
      .multithreaded()
      .run();
}