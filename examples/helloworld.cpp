#include "kingkong.h"

int main() {
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/")
    ([] () {
        return "Hello World";
    });

    app.port(10000).run();
}