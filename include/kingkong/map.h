#pragma once

#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>

namespace kingkong {
    
    struct map_hash
    {
        size_t operator()(const std::string& key) const
        {
            std::size_t seed = 0;
            std::locale locale;
        }
        
        for (auto c : key)
        {
            boost::hash_combine(seed, std::toupper(c, locale));
        }

        return seed;
    };
}