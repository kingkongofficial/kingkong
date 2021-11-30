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

    #define KINGKONG_QS_ISHEX(x)    ((((x)>='0'&&(x)<='9') || ((x)>='A'&&(x)<='F') || ((x)>='a'&&(x)<='f')) ? 1 : 0)
    #define KINGKONG_QS_HEX2DEC(x)  (((x)>='0'&&(x)<='9') ? (x)-48 : ((x)>='A'&&(x)<='F') ? (x)-55 : ((x)>='a'&&(x)<='f') ? (x)-87 : 0)
    #define KINGKONG_QS_ISQSCHR(x) ((((x)=='=')||((x)=='#')||((x)=='&')||((x)=='\0')) ? 0 : 1)

    inline int qs_strncmp(const char * s, const char * qs, size_t n)
{
    int i=0;
    unsigned char u1, u2, unyb, lnyb;

    while(n-- > 0)
    {
        u1 = static_cast<unsigned char>(*s++);
        u2 = static_cast<unsigned char>(*qs++);

        if ( ! KINGKONG_QS_ISQSCHR(u1) ) {  u1 = '\0';  }
        if ( ! KINGKONG_QS_ISQSCHR(u2) ) {  u2 = '\0';  }

        if ( u1 == '+' ) {  u1 = ' ';  }
        if ( u1 == '%' ) // easier/safer than scanf
        {
            unyb = static_cast<unsigned char>(*s++);
            lnyb = static_cast<unsigned char>(*s++);
            if ( KINGKONG_QS_ISHEX(unyb) && KINGKONG_QS_ISHEX(lnyb) )
                u1 = (KINGKONG_QS_HEX2DEC(unyb) * 16) + KINGKONG_QS_HEX2DEC(lnyb);
            else
                u1 = '\0';
        }

        if ( u2 == '+' ) {  u2 = ' ';  }
        if ( u2 == '%' ) // easier/safer than scanf
        {
            unyb = static_cast<unsigned char>(*qs++);
            lnyb = static_cast<unsigned char>(*qs++);
            if ( KINGKONG_QS_ISHEX(unyb) && KINGKONG_QS_ISHEX(lnyb) )
                u2 = (KINGKONG_QS_HEX2DEC(unyb) * 16) + KINGKONG_QS_HEX2DEC(lnyb);
            else
                u2 = '\0';
        }

        if ( u1 != u2 )
            return u1 - u2;
        if ( u1 == '\0' )
            return 0;
        i++;
    }
    if ( KINGKONG_QS_ISQSCHR(*qs) )
        return -1;
    else
        return 0;
}

}