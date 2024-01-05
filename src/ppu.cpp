#include "ppu.hpp"

// Constructor
PPU::PPU() {
    // Initialize registers
    x = y = 0;

    // Initialize memory to nullptr
    disk = nullptr;
}

// Destructor
PPU::~PPU() {}

// TODO: shared_ptr
void PPU::Mount(const Disk &disk) { this->disk = (Disk *)&disk; }
