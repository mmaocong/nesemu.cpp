#pragma once

#include <cstdint>
#include <vector>
#define MIN(a, b) ((a) < (b) ? (a) : (b))

using Byte = uint8_t;
using Word = uint16_t;
using Mem = std::vector<Byte>;

using RegB = uint8_t;  // byte-sized register
using RegW = uint16_t; // word-sized register

// Memory size of the main RAM (~16KB)
static constexpr uint32_t CPU_MEMSIZE = 0x4020;
