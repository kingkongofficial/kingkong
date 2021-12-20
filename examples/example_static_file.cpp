#include "kingkong.h"

int main()
{
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/")
    ([](const kingkong::request&, kingkong::response& res) {
        res.set_static_file_info("cat.jpg");
        res.end();
    });


    app.port(18080).run();


    return 0;
}