#pragma once

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <boost/optional.hpp>

namespace kingkong {
    int qs_strncmp(const char * s, const char * qs, size_t n);

    int qs_parse(char * qs, char * qs_kv[], int qs_kv_size);

    int qs_decode(char * qs);

    char * qs_k2v(const char * key, char * const * qs_kv, int qs_kv_size, int nth);

    char * qs_scanvalue(const char * key, const char * qs, char * val, size_t val_len);

    #undef _qsSORTING 
}