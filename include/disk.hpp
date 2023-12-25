// ============================================================================
// an abstract storage device, including the main memory, cartridge, and any
// other storage devices that may be needed in the future
// ============================================================================

#pragma once

#include "const.hpp"

struct Disk {
    // TODO: use shared_ptr
    Mem ram;
    Mem prg;
    Mem chr;
    // cartridge RAM not yet implemented

    // Constructor & Destructor
    Disk();
    ~Disk();

    // CPU read 1 byte
    Byte ReadCPU(const uint16_t &addr);

    // TODO PPU read 1 byte
    // Byte Read(const uint16_t &addr, PPU &ppu);
};
