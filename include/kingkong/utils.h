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

        constexpr const char* begin() const { return begin_; }
        constexpr const char* end() const { return begin_ + size_; }

        constexpr unsigned size() const
        {
            return size_; 
        }
    };

    constexpr unsigned find_closing_tag(const_str s, unsigned p)
    {
        return s[p] == '>' ? p : find_closing_tag(s, p + 1);
    }

    constexpr bool is_valid(const_str s, unsigned i = 0, int f = 0)
        {
            return i == s.size()   ? f == 0 :
                   f < 0 || f >= 2 ? false :
                   s[i] == '<'     ? is_valid(s, i + 1, f + 1) :
                   s[i] == '>'     ? is_valid(s, i + 1, f - 1) :
                                     is_valid(s, i + 1, f);
        }

    constexpr bool is_equ_p(const char* a, const char* b, unsigned n)
        {
            return *a == 0 && *b == 0 && n == 0 ? true :
                   (*a == 0 || *b == 0)         ? false :
                   n == 0                       ? true :
                   *a != *b                     ? false :
                                                  is_equ_p(a + 1, b + 1, n - 1);
        }

    constexpr bool is_equ_n(const_str a, unsigned ai, const_str b, unsigned bi, unsigned n)
        {
            return ai + n > a.size() || bi + n > b.size() ? false :
                   n == 0                                 ? true :
                   a[ai] != b[bi]                         ? false :
                                                            is_equ_n(a, ai + 1, b, bi + 1, n - 1);
        }

#endif

    }

}