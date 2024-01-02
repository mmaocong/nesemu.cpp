#pragma once

#include <string>

#include "const.hpp"

namespace Misc {

    // Get the hex representation of a number
    //
    // Args:
    //   n (uint32_t): number to convert
    //   d (uint8_t): number of digits
    //
    // Returns:
    //   std::string: hex representation of the number
    static inline std::string hex(uint32_t n, uint8_t d) {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

}; // namespace Misc
