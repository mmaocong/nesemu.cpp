// ============================================================================
// an abstract storage device, including the main memory, cartridge, and any
// other storage devices that may be needed in the future
// ============================================================================

#pragma once

#include "const.hpp"

enum class MirrorMode : uint8_t {
    SINGLE = 0,
    HORIZ,
    VERT,
    FOUR,
};

struct Disk {

    // Loopy's register
    union LoopyReg {
        struct {
            uint16_t coarse_x : 5;
            uint16_t coarse_y : 5;
            uint16_t nametable_x : 1;
            uint16_t nametable_y : 1;
            uint16_t fine_y : 3;
            uint16_t unused : 1;
        };
        uint16_t reg;
    };

    // TODO: use shared_ptr
    // TODO: expand PRG/CHR ROM

    Mem ram; // 8KB internal RAM + 8KB PPU registers + 32B APU & I/O registers
    Mem vrm; // NT * 4 + AT * 4 (4KB)
    Mem pal; // palette (32B)
    Mem prg; // max: 32KB
    Mem chr; // max: 8KB
    // Active address into NT to extract BG tile
    LoopyReg lrv;
    // Temporary store to be transferred to lrv from time to time
    LoopyReg lrt;
    uint16_t prg_kb;
    uint16_t chr_kb;
    MirrorMode mirror;

    // Constructor & Destructor
    Disk();
    ~Disk();

    // print disk info
    void Print();

    // Attach a cartridge
    void Attach(const std::string &);

    // CPU read 1 byte
    Byte ReadCPU(const uint16_t &);

    // CPU write 1 byte
    void WriteCPU(const uint16_t &, const Byte &);

    // PPU read 1 byte
    Byte ReadPPU(const uint16_t &);

    // PPU write 1 byte
    void WritePPU(const uint16_t &, const Byte &);
};
