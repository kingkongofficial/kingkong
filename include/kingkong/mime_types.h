#include <unordered_map>
#include <string>

namespace kingkong {
    const std::unordered_map<std::string, std::string> mine_types {
        {"html", "text/html"},
        {"css", "text/css"},
        {"xml", "text/xml"},
        {"gif", "image/gif"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"js", "application/javascript"},
        {"atom", "application/atom+xml"},
        {"rss", "application/rss+xml"},
        {"mml", "text/mathml"},
        {"txt", "text/plain"},
        {"jad", "text/vnd.sun.j2me.app-descriptor"},
        {"wml", "text/vnd.wap.wml"},
        {"htc", "text/x-component"},
        {"png", "image/png"},
        {"svgz", "image/svg+xml"},
        {"svg", "image/svg+xml"},
        {"tiff", "image/tiff"},
        {"tif", "image/tiff"},
        {"wbmp", "image/vnd.wap.wbmp"},
        {"webp", "image/webp"},
        {"ico", "image/x-icon"},
        {"jng", "image/x-jng"},
        {"bmp", "image/x-ms-bmp"},
        {"woff", "font/woff"},
        {"woff2", "font/woff2"},
    };
}
