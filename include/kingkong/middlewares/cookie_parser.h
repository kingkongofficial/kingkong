#pragma once

#include <boost/algorithm/string/trim.hpp>
#include "kingkong/http_request.h"
#include "kingkong/http_response.h"

namespace kinkong {
	struct CookieParser
	{
		struct context
		{
			std::unordered_map<std::string, std::string> jar;
			std::unordered_map<std::string, std::string> cookies_to_add;

			std::string get_cookie(const std::string& key) const
			{
				auto cookie = jar.find(key);
				if (cookie != jar.end())
					return cookie->second;
				return "";
			}

			void set_cookie(const std::string& key, const std::string& value)
			{
				cookies_to_add.emplace(key, value);
			}

			void before_handle(request& req, response& res, context& ctx)
			{
				int count = req.headers.count("Cookie");
				if (!count)
					return;
				if (count > 1)
				{
					res.code = 400;
					res.end();
					return;
				}
			}

		};
	};
}