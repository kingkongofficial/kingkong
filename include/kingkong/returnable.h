#pragma once

#include <string>

namespace kingkong {
    struct returnable 
    {
        std::string content_type;

        virtual std::string dump() const = 0;

        returnable(std::string ctype):
            content_type{ctype}
        {
        }

        virtual ~returnable(){};
    };
}