#include "kingkong_all.h"

#include <sstream>

class ExampleLogHandler : public kingkong::ILogHandler
{
public:
    void log(std::string, kingkong::LogLevel) override
    {
    }
};

int main()
{
    kingkong::SimpleApp app;

    KINGKONG_ROUTE(app, "/")
      .name("hello")([] {
          return "Hello World!";
      });

    KINGKONG_ROUTE(app, "/about")
    ([]() {
        return "About kingkong example.";
    });

    KINGKONG_ROUTE(app, "/json")
    ([] {
        kingkong::json::wvalue x({{"message", "Hello, World!"}});
        x["message2"] = "Hello, World.. Again!";
        return x;
    });

    KINGKONG_ROUTE(app, "/json-initializer-list-constructor")
    ([] {
        return kingkong::json::wvalue({
          {"first", "Hello world!"},                     
          {"second", std::string("How are you today?")}, 
          {"third", 54},                                 
          {"fourth", 54l},                               
          {"fifth", 54u},                                
          {"sixth", 54ul},                               
          {"seventh", 2.f},                              
          {"eighth", 2.},                                
          {"ninth", nullptr},                            
          {"tenth", true}                                
        });
    });

    KINGKONG_ROUTE(app, "/json_list")
    ([] {
        kingkong::json::wvalue x(kingkong::json::wvalue::list({1, 2, 3}));
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
    ([](const kingkong::request& req) {
        auto x = kingkong::json::load(req.body);
        if (!x)
            return kingkong::response(400);
        int sum = x["a"].i() + x["b"].i();
        std::ostringstream os;
        os << sum;
        return kingkong::response{os.str()};
    });

    KINGKONG_ROUTE(app, "/params")
    ([](const kingkong::request& req) {
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
      .server_name("KingkongCpp")
      .multithreaded()
      .run();
}