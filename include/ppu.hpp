// ============================================================================
// PPU
//
// References:
//
// - https://www.nesdev.org/wiki/PPU_registers
// ============================================================================

#pragma once

#include "const.hpp"
#include "disk.hpp"

struct PPU {

    RegW y; // scanline
    RegW x; // cycle

    Disk *disk;

    // Constructor & Destructor
    PPU();
    ~PPU();

    // ------------------------------------------------------------------------
    // functions
    // ------------------------------------------------------------------------

    // attach memory to the CPU
    void Mount(const Disk &disk);

    // ---------- execute functions ----------

    void RunCycle();
};
