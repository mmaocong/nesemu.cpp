#pragma once

#include "const.hpp"

// Cassette Header
struct CHeader {
    char name[4];
    uint8_t n_chunk_prg;
    uint8_t n_chunk_chr;
    uint8_t mapper1;
    uint8_t mapper2;
    uint8_t prg_ram_size;
    uint8_t tv_system1;
    uint8_t tv_system2;
    char unused[5];
};

namespace Cassette {

    CHeader Load(const std::string &, Mem &, Mem &);

}; // namespace Cassette
