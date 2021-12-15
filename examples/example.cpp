#include "kingkong.h"
#include <sstream>

class ExampleLogHandler : public kingkong::ILogHandler
{
public:
    void log(std::string, kingkong::LogLevel) override
    {
    }
};

struct ExampleMiddleware
{
    std::string message;

    ExampleMiddleware():
        message("foo")
    {
    }

    void setMessage(const std::string& newMsg)
    {
        message = newMsg;
    }

    struct context 
    {
    };

    void before_handle(kingkong::request&, kingkong::response&, context&)
    {
        KINGKONG_LOG_LEVEL << " - Message: " << message;
    }

    void after_handle(kingkong::request&, kingkong::response&, context&)
    {

    }
}

int main()
{
    kingkong::App<ExampleMiddleware> app;

    app.get_middleware<ExampleMiddleware>().setMessage("hello");

    KINGKONG_ROUTE(app, "/")
        .name("hello")([] {
            return "Helllo world";
        });
}