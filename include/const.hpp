#pragma once

#include <cstdint>
#include <vector>

using Byte = uint8_t;
using Word = uint16_t;
using Mem = std::vector<Byte>;

// Memory size of the main RAM (~16KB)
static constexpr uint32_t CPU_MEMSIZE = 0x4020;
