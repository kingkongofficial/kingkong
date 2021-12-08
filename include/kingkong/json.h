#pragma once 

#include <string>
#ifndef KINGKONG_JSON_USE_MAP
#include <map>
#else
#include <unordered_map>
#endif
#include <iostream>
#include <algorithm>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/operators.hpp>
#include <vector>
#include "kingkong/settings.h"
#include "kingkong/returnable.h"

#if defined(__GNUG__) || defined(__clang__)
#define kingkong_json_likely(x) __builtin_expect(x, 1)
#define kingkong_json_unlikely(x) __builtin_expect(x, 0)
#else
#define kingkong_json_likely(x) x
#define kingkong_json_unlikely(x) x
#endif

namespace kingkong {

    namespace mustache {
        class template_t;
    }

    namespace json
    {
        inline void escape(const std::string& str, std::string& ret)
        {
            ret.reserve(ret.size())
        }
        enum class type : char
        {
            Null,
            False,
            True,
            Number,
            String,
            List,
            Object,
        };
    }

}