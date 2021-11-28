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

#endif

    }

}