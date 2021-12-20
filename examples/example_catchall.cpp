#include <kingkong.h>

int main()
{
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/")
    ([]() {
        return "Hello";
    });

    KINGKONG_CATCHALL_ROUTE(app)
    ([](kingkong::response& res) {
        if (res.code == 404)
        {
            res.body = "The URL does not seem to be correct.";
        }
        else if (res.code == 405)
        {
            res.body = "The HTTP method does not seem to be correct.";
        }
        res.end();
    });

    app.port(18080).run();
}