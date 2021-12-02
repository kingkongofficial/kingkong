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
		}
	};
}