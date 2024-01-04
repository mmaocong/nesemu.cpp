#pragma once

#include "const.hpp"

// Cassette Header
struct NesHdr {
    char name[4];
    uint8_t n_chunk_prg;
    uint8_t n_chunk_chr;

    // flags 6:
    //
    //   7654 3210
    //   ---- ----
    //   |||| ||||
    //   |||| |||+- Mirroring: 0: horizontal; 1: vertical
    //   |||| |||
    //   |||| ||+-- 1: Cartridge contains battery-backed PRG RAM ($6000-7FFF)
    //   |||| ||       or other persistent memory
    //   |||| ||
    //   |||| |+--- 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
    //   |||| |
    //   |||| +---- 1: Ignore mirroring bit; instead provide four-screen VRAM
    //   ||||
    //   ++++------ Lower nybble of mapper number
    uint8_t vertical : 1;
    uint8_t battery : 1;
    uint8_t trainer : 1;
    uint8_t four : 1;
    uint8_t mapper_lo : 4;

    // flags 7:
    //
    //   7654 3210
    //   ---- ----
    //   |||| ||||
    //   |||| |||+- VS Unisystem
    //   |||| |||
    //   |||| ||+-- PlayChoice-10 (8 KB of Hint Screen data stored after CHR
    //   |||| ||    data)
    //   |||| ||
    //   |||| ++--- If equal to 2, flags 8-15 are in NES 2.0 format
    //   ||||
    //   ++++----- Upper nybble of mapper number
    uint8_t vs : 1;
    uint8_t pc10 : 1;
    uint8_t nes2 : 2;
    uint8_t mapper_hi : 4;

    uint8_t prg_ram_size; // often not used or inaccurate

    // flags 9:
    uint8_t pal : 1; // 0: NTSC; 1: PAL
    uint8_t unused1 : 7;

    uint8_t flags10; // TODO: NES 2.0
    char unused2[5];

    void Print() const;
};
