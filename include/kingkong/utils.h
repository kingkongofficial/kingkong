#pragma once

#include "kingkong/settings.h"

namespace kingkong  {
    
    namespace magic {
#ifndef KINGKONG_MSVC_WORKAROUND
    struct OutOfRange
    {
        OutOfRange(unsigned, unsigned) {}
    };

    constexpr unsigned requires_in_range(unsigned i, unsigned len)
    {
        return i >= len ? throw OutOfRange(i, len) : i;
    };

    class const_str
    {
        const char* const begin_;
        unsigned size_;
        
    public:
        template<unsigned N>
        constexpr const_str(const char (&arr)[N]):
            begin_(arr), size_(N - 1)
        {
            static_assert(N >= 1, "not a string");
        }

        constexpr char operator[](unsigned i) const
        {
            return requires_in_range(i, size_), begin_[i];
        }

        constexpr operator const char*() const
        {
            return begin_;
        }
    }

#endif

    }

}