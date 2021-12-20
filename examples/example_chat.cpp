#include "kingkong.h"
#include <string>
#include <vector>
#include <chrono>

using namespace std;

vector<string> msgs;
vector<pair<kingkong::response*, decltype(chrono::steady_clock::now())>> ress;

void broadcast(const string& msg)
{
    msgs.push_back(msg);
    kingkong::json::wvalue x;
    x["msgs"][0] = msgs.back();
    x["last"] = msgs.size();
    string body = x.dump();
    for (auto p : ress)
    {
        auto* res = p.first;
        KINGKONG_LOG_DEBUG << res << " replied: " << body;
        res->end(body);
    }
    ress.clear();
}

int main()
{
    kingkong::SimpleApp app;
    kingkong::mustache::set_base(".");

    KINGKONG_ROUTE(app, "/")
    ([] {
        kingkong::mustache::context ctx;
        return kingkong::mustache::load("example_chat.html").render();
    });

    KINGKONG_ROUTE(app, "/logs")
    ([] {
        KINGKONG_LOG_INFO << "logs requested";
        kingkong::json::wvalue x;
        int start = max(0, (int)msgs.size() - 100);
        for (int i = start; i < (int)msgs.size(); i++)
            x["msgs"][i - start] = msgs[i];
        x["last"] = msgs.size();
        KINGKONG_LOG_INFO << "logs completed";
        return x;
    });

    KINGKONG_ROUTE(app, "/logs/<int>")
    ([](const kingkong::request& /*req*/, kingkong::response& res, int after) {
        KINGKONG_LOG_INFO << "logs with last " << after;
        if (after < (int)msgs.size())
        {
            kingkong::json::wvalue x;
            for (int i = after; i < (int)msgs.size(); i++)
                x["msgs"][i - after] = msgs[i];
            x["last"] = msgs.size();

            res.write(x.dump());
            res.end();
        }
        else
        {
            vector<pair<kingkong::response*, decltype(chrono::steady_clock::now())>> filtered;
            for (auto p : ress)
            {
                if (p.first->is_alive() && chrono::steady_clock::now() - p.second < chrono::seconds(30))
                    filtered.push_back(p);
                else
                    p.first->end();
            }
            ress.swap(filtered);
            ress.push_back({&res, chrono::steady_clock::now()});
            KINGKONG_LOG_DEBUG << &res << " stored " << ress.size();
        }
    });

    KINGKONG_ROUTE(app, "/send")
      .methods("GET"_method, "POST"_method)([](const kingkong::request& req) {
          KINGKONG_LOG_INFO << "msg from client: " << req.body;
          broadcast(req.body);
          return "";
      });

    app.port(40080)
      .run();
}