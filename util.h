#ifndef __NANOM_UTILS_H
#define __NANOM_UTILS_H

#include <stdlib.h>

#include "nanom.h"


namespace nanom {

inline std::string int_to_string(Int i) {
    static char buff[1024];
    ::snprintf(buff, 1023, "%lld", i);
    return buff;
}

inline std::string uint_to_string(UInt i) {
    static char buff[1024];
    ::snprintf(buff, 1023, "%llu", i);
    return buff;
}

inline std::string real_to_string(Real i) {
    static char buff[1024];
    ::snprintf(buff, 1023, "%g", i);
    return buff;
}

inline Int string_to_int(const std::string& s) {
    return (Int)::strtoll(s.c_str(), NULL, 10);
}

inline UInt string_to_uint(const std::string& s) {
    return (UInt)::strtoul(s.c_str(), NULL, 10);
}

inline Real string_to_real(const std::string& s) {
    return (Real)::strtod(s.c_str(), NULL);
}


}


#endif
