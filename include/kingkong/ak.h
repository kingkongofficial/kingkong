#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include "kingkong/utils.h"

namespace kingkong {
    
    enum class HTTPMethod : char
    {
#ifndef DELETE
#endif
    };

    enum status
    {
        CONTINUE = 100,
        SWITCHING_PROTOCOLS = 101,
        OK = 200,
    }
}