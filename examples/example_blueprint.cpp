#include "kingkong.h"

int main()
{
    kingkong::SimpleApp app;

    kingkong::Blueprint bp("bp_prefix", "cstat", "ctemplate");


    kingkong::Blueprint sub_bp("bp2", "csstat", "cstemplate");

    KINGKONG_BP_ROUTE(sub_bp, "/")
    ([]() {
        return "Hello world!";
    });

    KINGKONG_BP_CATCHALL_ROUTE(sub_bp)
    ([]() {
        return "WRONG!!";
    });


    bp.register_blueprint(sub_bp);
    app.register_blueprint(bp);

    app.loglevel(kingkong::LogLevel::Debug).port(18080).run();
}