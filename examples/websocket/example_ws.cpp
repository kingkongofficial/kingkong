#include "kingkong.h"
#include <unordered_set>
#include <mutex>


int main()
{
    kingkong::SimpleApp app;

    std::mutex mtx;
    std::unordered_set<kingkong::websocket::connection*> users;

    KINGKONG_ROUTE(app, "/ws")
      .websocket()
      .onopen([&](kingkong::websocket::connection& conn) {
          KINGKONG_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
          users.insert(&conn);
      })
      .onclose([&](kingkong::websocket::connection& conn, const std::string& reason) {
          KINGKONG_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
          users.erase(&conn);
      })
      .onmessage([&](kingkong::websocket::connection&, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);
          for (auto u : users)
              if (is_binary)
                  u->send_binary(data);
              else
                  u->send_text(data);
      });

    KINGKONG_ROUTE(app, "/")
    ([] {
        char name[256];
        gethostname(name, 256);
        kingkong::mustache::context x;
        x["servername"] = name;

        auto page = kingkong::mustache::load("ws.html");
        return page.render(x);
    });

    app.port(40080)
      .multithreaded()
      .run();
}