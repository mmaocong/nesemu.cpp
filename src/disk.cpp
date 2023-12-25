#include "disk.hpp"
#include "const.hpp"

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// address range check for CPU
enum AddrRangeCPU : uint8_t {
    RG_NONE = 0, // invalid

    // RAM space
    RG_2000 = 1, // 0x0000 - 0x1fff, main RAM
    RG_4000 = 2, // 0x2000 - 0x3fff, PPU registers
    RG_4020 = 3, // 0x4000 - 0x401f, APU

    // cartridge space
    RG_6000 = 4, // 0x4020 - 0x5fff, expansion ROM
    RG_8000 = 5, // 0x6000 - 0x7fff, expansion RAM
    RG_G000 = 6, // 0x8000 - 0xffff, PRG-ROM
};

static constexpr AddrRangeCPU addr_range_cpu(const uint16_t &addr) {
    if (addr < 0x2000) {
        return RG_2000;
    } else if (addr < 0x4000) {
        return RG_4000;
    } else if (addr < 0x4020) {
        return RG_4020;
    } else if (addr < 0x6000) {
        return RG_6000;
    } else if (addr < 0x8000) {
        return RG_8000;
    } else if (addr <= 0xFFFF) {
        return RG_G000;
    } else {
        return RG_NONE;
    }
}

// ----------------------------------------------------------------------------
// Disk Class
// ----------------------------------------------------------------------------

// Constructor
Disk::Disk() : ram(CPU_MEMSIZE, 0), prg(0), chr(0) {}

// Destructor
Disk::~Disk() {}

Byte Disk::ReadCPU(const uint16_t &addr) {
    AddrRangeCPU rg = addr_range_cpu(addr);
    switch (rg) {
    case RG_2000:
        return ram[addr & 0x07FF];
    case RG_4000:
        // NOTE: PPU registers are mirrored every 8 bytes i.e.
        // 0x2000 == 0x2008 == 0x2010 == ...
        // 0x2001 == 0x2009 == 0x2011 == ...
        // ...
        // `addr & 0x2007` equals to `addr % 8 + 0x2000`
        return ram[addr & 0x2007];
    case RG_4020:
        // TODO: verify this
        return ram[addr & 0x1F];
    case RG_6000:
        // not implemented
        break;
    case RG_8000:
        // not implemented
        break;
    case RG_G000:
        if (prg.size() < 0x8000) {
            // 16KB PRG-ROM
            return prg[(addr - 0x8000) & 0x3FFF];
        } else {
            // 32KB PRG-ROM
            return prg[addr - 0x8000];
        }
    default:
        break;
    }
    return 0;
}
