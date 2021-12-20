#include "kingkong.h"
#include <sstream>

class ExampleLogHandler : public kingkong::ILogHandler
{
public:
    void log(std::string message, kingkong::LogLevel level) override
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
    {};

    void before_handle(kingkong::request& req, kingkong::response& res, context& ctx)
    {
        KINGKONG_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(kingkong::request& req, kingkong::response& res, context& ctx)
    {
    }
};

int main()
{
    kingkong::App<ExampleMiddleware> app;

    app.get_middleware<ExampleMiddleware>().setMessage("hello");

    KINGKONG_ROUTE(app, "/")
      .name("hello")([] {
          return "Hello World!";
      });

    KINGKONG_ROUTE(app, "/about")
    ([]() {
        return "About kingkong example.";
    });

    KINGKONG_ROUTE(app, "/path/")
    ([]() {
        return "Trailing slash test case..";
    });

    KINGKONG_ROUTE(app, "/json")
    ([] {
        kingkong::json::wvalue x;
        x["message"] = "Hello, World!";
        return x;
    });

    KINGKONG_ROUTE(app, "/hello/<int>")
    ([](int count) {
        if (count > 100)
            return kingkong::response(400);
        std::ostringstream os;
        os << count << " bottles of beer!";
        return kingkong::response(os.str());
    });

    KINGKONG_ROUTE(app, "/add/<int>/<int>")
    ([](kingkong::response& res, int a, int b) {
        std::ostringstream os;
        os << a + b;
        res.write(os.str());
        res.end();
    });

    KINGKONG_ROUTE(app, "/add_json")
      .methods(kingkong::HTTPMethod::Post)([](const kingkong::request& req) {
          auto x = kingkong::json::load(req.body);
          if (!x)
              return kingkong::response(400);
          auto sum = x["a"].i() + x["b"].i();
          std::ostringstream os;
          os << sum;
          return kingkong::response{os.str()};
      });

    app.route_dynamic("/params")([](const kingkong::request& req) {
        std::ostringstream os;
        os << "Params: " << req.url_params << "\n\n";
        os << "The key 'foo' was " << (req.url_params.get("foo") == nullptr ? "not " : "") << "found.\n";
        if (req.url_params.get("pew") != nullptr)
        {
            double countD = boost::lexical_cast<double>(req.url_params.get("pew"));
            os << "The value of 'pew' is " << countD << '\n';
        }
        auto count = req.url_params.get_list("count");
        os << "The key 'count' contains " << count.size() << " value(s).\n";
        for (const auto& countVal : count)
        {
            os << " - " << countVal << '\n';
        }
        return kingkong::response{os.str()};
    });

    kingkong::logger::setLogLevel(kingkong::LogLevel::Debug);

    app.port(18080)
      .multithreaded()
      .run();
}