#include "disk.hpp"

// ----------------------------------------------------------------------------
// Address Ranges
// ----------------------------------------------------------------------------

// Address Range for CPU:
//
//   =========================== 0xG000: Cartridge Space.
//     PRG ROM (Upper Bank).
//    -------------------------  0xC000:
//     PRG ROM (Lower Bank).
//    -------------------------  0x8000:
//     PRG RAM.
//    -------------------------  0x6000:
//     Expansion Modules.
//   =========================== 0x4020: Disabled.
//      ...
//   =========================== 0x4018: APU and I/O Registers.
//      ...
//    - - - - - - - - - - - - -  0x4015:
//      OAMDMA
//    - - - - - - - - - - - - -  0x4014:
//      ...
//   =========================== 0x4000: PPU Registers.
//      Mirrors of $2000–$2007
//     (repeats every 8 bytes).
//    -------------------------  0x2008:
//      PPUDATA.
//    - - - - - - - - - - - - -  0x2007:
//      PPUADDR.
//    - - - - - - - - - - - - -  0x2006:
//      PPUSCROLL.
//    - - - - - - - - - - - - -  0x2005:
//      OAMDATA.
//    - - - - - - - - - - - - -  0x2004:
//      OAMADDR.
//    - - - - - - - - - - - - -  0x2003:
//      PPUSTATUS.
//    - - - - - - - - - - - - -  0x2002:
//      PPUMASK.
//    - - - - - - - - - - - - -  0x2001:
//      PPUCTRL.
//   =========================== 0x2000: RAM.
//      Mirror 2.
//    - - - - - - - - - - - - -  0x1800:
//      Mirror 1.
//    - - - - - - - - - - - - -  0x1000:
//      Mirror 0.
//    - - - - - - - - - - - - -  0x0800:
//      2kB Internal RAM.
//   =========================== 0x0000:
//
// NOTE: read / write to the PPU registers will affect the contents of the
//       PPU and Loopy's registers.
//
// TODO: implement read / write effects
// https://github.com/quackenbush/nestalgia/blob/master/docs/ppu/SKINNY.TXT
//
enum class AddrRangeMBus : uint8_t {
    RG_NONE = 0, // invalid

    // RAM space
    RG_2000 = 1, // 0x0000 - 0x1fff, main RAM
    RG_4000 = 2, // 0x2000 - 0x3fff, PPU registers
    RG_4020 = 3, // 0x4000 - 0x401f, APU + OAMDMA

    // cartridge space
    RG_6000 = 4, // 0x4020 - 0x5fff, expansion ROM
    RG_8000 = 5, // 0x6000 - 0x7fff, expansion RAM
    RG_G000 = 6, // 0x8000 - 0xffff, PRG-ROM
};

static inline constexpr AddrRangeMBus addr_range_cpu(const uint16_t &addr) {
    if (addr < 0x2000) {
        return AddrRangeMBus::RG_2000;
    } else if (addr < 0x4000) {
        return AddrRangeMBus::RG_4000;
    } else if (addr < 0x4020) {
        return AddrRangeMBus::RG_4020;
    } else if (addr < 0x6000) {
        return AddrRangeMBus::RG_6000;
    } else if (addr < 0x8000) {
        return AddrRangeMBus::RG_8000;
    } else if (addr <= 0xFFFF) {
        return AddrRangeMBus::RG_G000;
    } else {
        return AddrRangeMBus::RG_NONE;
    }
}

// Address Range for PPU:
//
//   ============================ 0x4000:
//    Mirrors of $3F00-$3F1F.
//   ============================ 0x3F20: Palette Table, containing master
//      FG Palette 3.                     palette indices.
//    - - - - - - - - - - - - - - 0x3F1D:
//      Mirror of $3F0C.
//    - - - - - - - - - - - - - - 0x3F1C:
//      FG Palette 2.
//    - - - - - - - - - - - - - - 0x3F19:
//      Mirror of $3F08.
//    - - - - - - - - - - - - - - 0x3F18:
//      FG Palette 1.
//    - - - - - - - - - - - - - - 0x3F15:
//      Mirror of $3F04.
//    - - - - - - - - - - - - - - 0x3F14:
//      FG Palette 0.
//    - - - - - - - - - - - - - - 0x3F11:
//      Mirror of $3F00.
//   ---------------------------- 0x3F10:
//      BG Palette 3.
//    - - - - - - - - - - - - - - 0x3F0D:
//      Mirror of $3F00.
//    - - - - - - - - - - - - - - 0x3F0C:
//      BG Palette 2.
//    - - - - - - - - - - - - - - 0x3F09:
//      Mirror of $3F00.
//    - - - - - - - - - - - - - - 0x3F08:
//      BG Palette 1.
//    - - - - - - - - - - - - - - 0x3F05:
//      Mirror of $3F00.
//    - - - - - - - - - - - - - - 0x3F04:
//      BG Palette 0.
//    - - - - - - - - - - - - - - 0x3F01:
//      Universal BG Color.
//   ============================ 0x3F00: Name Tables (VRAM).
//    Mirrors of $2000-$2EFF.
//   ---------------------------- 0x3000:   - Logical NT 3.
//      AT 3.
//    - - - - - - - - - - - - - - 0x2FC0:
//      NT 3.
//   ---------------------------- 0x2C00:   - Logical NT 2.
//      AT 2.
//    - - - - - - - - - - - - - - 0x2BC0:
//      NT 2.
//   ---------------------------- 0x2800:   - Logical NT 1.
//      AT 1.
//    - - - - - - - - - - - - - - 0x27C0:
//      NT 1.
//   ---------------------------- 0x2400:   - Logical NT 0.
//      AT 0.
//    - - - - - - - - - - - - - - 0x23C0:
//      NT 0 32x30 8-bit indices.
//   ============================ 0x2000: CHR ROM / RAM.
//    Pattern Table 1.
//   ---------------------------- 0x1000:
//    Pattern Table 0.
//   ============================ 0x0000:
enum class AddrRangePBus : uint8_t {
    RG_NONE = 0, // invalid

    // CHR ROM
    RG_1000 = 1, // 0x0000 - 0x0fff, CHR pattern table 0
    RG_2000 = 2, // 0x1000 - 0x1fff, CHR pattern table 1

    // VRAM
    RG_3000 = 3, // 0x2000 - 0x2fff, VRAM NT 0-3 / AT 0-3

    // mirror of VRAM
    RG_3F00 = 4, // 0x3000 - 0x3eff, mirror of VRAM 0x2000 - 0x2eff

    // palette RAM, indices to master palette
    RG_3F20 = 5, // 0x3f00 - 0x3f1f, universal BG + BG / FG palette 0-3

    // mirror of palette RAM:
    // 0x3f20 - 0x3fff ==> 0x3f00 - 0x3f1f
    // - src: 0011 1111 0010 0000, 0011 1111 1111 1111
    // - AND: 0011 1111 0001 1111, 0011 1111 0001 1111 (0x3f1f)
    // - dst: 0011 1111 0000 0000, 0011 1111 0001 1111
    RG_4000 = 6,
};

static inline constexpr AddrRangePBus addr_range_ppu(const uint16_t &addr) {
    if (addr < 0x1000) {
        return AddrRangePBus::RG_1000;
    } else if (addr < 0x2000) {
        return AddrRangePBus::RG_2000;
    } else if (addr < 0x3000) {
        return AddrRangePBus::RG_3000;
    } else if (addr < 0x3F00) {
        return AddrRangePBus::RG_3F00;
    } else if (addr < 0x3F20) {
        return AddrRangePBus::RG_3F20;
    } else if (addr < 0x4000) {
        return AddrRangePBus::RG_4000;
    } else {
        return AddrRangePBus::RG_NONE;
    }
}

// ----------------------------------------------------------------------------
// Main Bus Access
// ----------------------------------------------------------------------------

// Read the PPU Registers `PRam`
//
// NOTE: The input address `addr` MUST be in the range of 0x0000 - 0x0007.
Byte Disk::ReadPRam(const uint16_t &addr) {
    Byte data = 0x00; // dummy default value
    switch (addr) {
    case 0x0000: // PPUCTRL: not readable
        break;
    case 0x0001: // PPUMASK: not readable
        break;
    case 0x0002: // PPUSTATUS
        data = pram.status.reg;
        // Reading PPUSTATUS resets different parts of the circuit.
        // - resets write toggle
        // - resets vblank flag
        pram.w = 0;
        pram.status.vbk = 0;
        break;
    case 0x0003: // OAMADDR: not readable
        break;
    case 0x0004: // OAMDATA: read directly
        data = pram.oamdata;
    case 0x0005: // PPUSCROLL: not readable
        break;
    case 0x0006: // PPUADDR: not readable
        break;
    case 0x0007: // PPUDATA
        // - Access the NT VRAM get delayed one cycle
        // - output buffer containing data from the previous read request
        // - update the buffer for next time
        data = pram.buffer;
        pram.buffer = ReadPBus(pram.v.reg);
        // - NO DELAY when reading the palette
        if (addr_range_ppu(pram.v.reg) >= AddrRangePBus::RG_3F20) {
            data = pram.buffer;
        }
        // NOTE: All reads / writes PPUDATA automatically increment the VRAM
        //       address by 1 or 32 depending on the PPUCTRL register.
        pram.v.reg += pram.AddrInc();
        break;
    default:
        break;
    }
    return data;
}

// Write the PPU Registers `PRam`
//
// NOTE: The input address `addr` MUST be in the range of 0x0000 - 0x0007.
void Disk::WritePRam(const uint16_t &addr, const Byte &data) {
    switch (addr) {
    case 0x0000: // PPUCTRL
        pram.ctrl.reg = data;
        pram.t.ntx = pram.ctrl.ntx;
        pram.t.nty = pram.ctrl.nty;
        break;
    case 0x0001: // PPUMASK
        pram.mask.reg = data;
        break;
    case 0x0002: // PPUSTATUS: not writable
        break;
    case 0x0003: // OAMADDR
        pram.oamaddr = data;
        break;
    case 0x0004: // OAMDATA
        pram.oamdata = data;
        break;
    case 0x0005: // PPUSCROLL
        // data contains the scroll offsets in pixel, which can be split into
        // coarse and fine components.
        if (pram.w == 0) {
            // 1st write:
            pram.x = data & 0x07;
            pram.t.x_coarse = data >> 3;
            pram.w = 1;
        } else {
            // second write
            pram.t.y_coarse = data >> 3;
            pram.t.y = data & 0x07;
            pram.w = 0;
        }
        break;
    case 0x0006: // PPUADDR
        // data contains the address of the VRAM to access
        if (pram.w == 0) {
            // 1st write:
            // - The PPU bus can be accessed by CPU via the PPUADDR and PPUDATA
            //   registers.
            // - The PPU Bus uses 14-bit addresses (0x0000 - 0x3FFF).
            // - Writes are stored in the T register.
            // - The 1st write to this register writes the high 6 bits
            pram.t.reg = (uint16_t)(data & 0x3F) << 8 | (pram.t.reg & 0x00FF);
            pram.w = 1;
        } else {
            // - The 2nd write to this register writes the low 8 bits
            // - When a whole address has been written, the PPU will
            //   update its V register and reset the write toggle.
            // - NOTE: It is NOT wise to write to the PPU during rendering as
            //   the PPU will maintam the address automatically whilst
            //   rendering the scanline position.
            pram.t.reg = (pram.t.reg & 0xFF00) | data;
            pram.v.reg = pram.t.reg;
            pram.w = 0;
        }
        break;
    case 0x0007: // PPUDATA
        // - The PPU bus can be accessed by CPU via the PPUADDR and PPUDATA
        //   registers.
        // - The PPU Bus uses 14-bit addresses (0x0000 - 0x3FFF).
        // - Writes are stored in the T register.
        // - The 1st write to this register writes the high 6 bits
        // - The 2nd write to this register writes the low 8 bits
        // - When a whole address has been written, the PPU will
        //   update its V register and reset the write toggle.
        // - NOTE: It is NOT wise to write to the PPU during rendering as
        //   the PPU will maintam the address automatically whilst
        //   rendering the scanline position.
        WritePBus(pram.v.reg, data);
        // NOTE: All reads / writes PPUDATA automatically increment the VRAM
        //       address by 1 or 32 depending on the PPUCTRL register.
        pram.v.reg += pram.AddrInc();
        break;
    default:
        break;
    }
}

// TODO: log read access
Byte Disk::ReadMBus(const uint16_t &addr) {
    AddrRangeMBus rg = addr_range_cpu(addr);
    switch (rg) {
    case AddrRangeMBus::RG_2000:
        // NOTE: 2KB RAM
        return ram[addr & 0x07FF];
    case AddrRangeMBus::RG_4000:
        // NOTE: PPU registers are mirrored every 8 bytes i.e.
        // 0x2000 == 0x2008 == 0x2010 == ...
        // 0x2001 == 0x2009 == 0x2011 == ...
        // ...
        return ReadPRam(addr & 0x0007);
    case AddrRangeMBus::RG_4020:
        return ram[addr];
    case AddrRangeMBus::RG_6000:
        // not implemented
        break;
    case AddrRangeMBus::RG_8000:
        // not implemented
        break;
    case AddrRangeMBus::RG_G000:
        if (prg_kb < 32) {
            // 16KB PRG-ROM
            return prg[(addr - 0x8000) & 0x3FFF];
        } else {
            // 32KB PRG-ROM
            // TODO: above 32KB?
            return prg[addr - 0x8000];
        }
    default:
        break;
    }
    return 0;
}

// TODO: log write access
void Disk::WriteMBus(const uint16_t &addr, const Byte &data) {
    AddrRangeMBus rg = addr_range_cpu(addr);
    switch (rg) {
    case AddrRangeMBus::RG_2000:
        // NOTE: 2KB RAM
        ram[addr & 0x07FF] = data;
        break;
    case AddrRangeMBus::RG_4000:
        // NOTE: PPU registers are mirrored every 8 bytes i.e.
        // 0x2000 == 0x2008 == 0x2010 == ...
        // 0x2001 == 0x2009 == 0x2011 == ...
        // ...
        WritePRam(addr & 0x0007, data);
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
// PPU Bus Access
// ----------------------------------------------------------------------------

// Map the address to the VRAM based on the mirroring mode
//
// The input address `addr` MUST be in the range of 0x0000 - 0x0FFF.
//
// - 4-screen mode: the address is not mirrored.
// - access to Logical NT 0 and NT 3 will not be mirrored, whatever the mode is
// - Horizontal mode: NT 1 and NT 2 are mirrored to NT 0 and NT 3 respectively.
//   - NT1: 0x0400 - 0x07FF ==> 0x0000 - 0x03FF:
//     - AND 0x03FF
//   - NT2: 0x0800 - 0x0BFF ==> 0x0C00 - 0x0FFF
//     - src: 1000 0000 0000, 1011 1111 1111
//     - +OR: 0100 0000 0000, 0100 0000 0000 (0x0400)
//     - dst: 1100 0000 0000, 1111 1111 1111
// - Vertical mode: NT 1 and NT 2 are mirrored to NT 3 and NT 0 respectively.
//   - NT1: 0x0400 - 0x07FF ==> 0x0C00 - 0x0FFF
//     - src: 0100 0000 0000, 0111 1111 1111
//     - +OR: 1000 0000 0000, 1000 0000 0000 (0x0800)
//     - dst: 1100 0000 0000, 1111 1111 1111
//   - NT2: 0x0800 - 0x0BFF ==> 0x0000 - 0x03FF
//     - AND 0x03FF
static inline uint16_t vram_addr_mapper(const uint16_t &addr,
                                        const MirrorMode &mode) {
    switch (mode) {
    case MirrorMode::SINGLE:
        return addr & 0x03FF;
    case MirrorMode::FOUR:
        return addr;
    case MirrorMode::HORIZ:
        if (addr < 0x0400) {
            return addr;
        } else if (addr < 0x0800) {
            return addr & 0x03FF;
        } else if (addr < 0x0C00) {
            return addr | 0x0400;
        } else {
            return addr;
        }
    case MirrorMode::VERT:
        if (addr < 0x0400) {
            return addr;
        } else if (addr < 0x0800) {
            return addr | 0x0800;
        } else if (addr < 0x0C00) {
            return addr & 0x03FF;
        } else {
            return addr;
        }
    default:
        throw std::runtime_error("invalid mirror mode");
    }
}

static inline Byte read_ppu_vram(const uint16_t &addr, const Mem &vram,
                                 const MirrorMode &mode) {
    const uint16_t mapped_addr = vram_addr_mapper(addr, mode);
    return vram[mapped_addr];
}

static inline void write_ppu_vram(const uint16_t &addr, const Byte &data,
                                  Mem &vram, const MirrorMode &mode) {
    const uint16_t mapped_addr = vram_addr_mapper(addr, mode);
    vram[mapped_addr] = data;
}

// Map the address to the palette table based on the mirroring mechanism.
// The input address `addr` MUST be in the range of 0x0000 - 0x001F.
//
// 0x0010, 0x0014, 0x0018, 0x001C will be mirrored to
// 0x0000, 0x0004, 0x0008, 0x000C respectively
static inline uint16_t pal_addr_mapper(const uint16_t &addr) {
    if (addr == 0x0010 || addr == 0x0014 || addr == 0x0018 || addr == 0x001C) {
        return addr & 0x000C;
    } else {
        return addr;
    }
}

static inline Byte read_ppu_pal(const uint16_t &addr, const Mem &mem) {
    const uint16_t mapped_addr = pal_addr_mapper(addr);
    return mem[mapped_addr];
}

static inline void write_ppu_pal(const uint16_t &addr, const Byte &data,
                                 Mem &mem) {
    const uint16_t mapped_addr = pal_addr_mapper(addr);
    mem[mapped_addr] = data;
}

Byte Disk::ReadPBus(const uint16_t &addr) {
    AddrRangePBus rg = addr_range_ppu(addr);
    switch (rg) {
    case AddrRangePBus::RG_1000:
        return chr[addr];
    case AddrRangePBus::RG_2000:
        return chr[addr];
    case AddrRangePBus::RG_3000:
        return read_ppu_vram(addr & 0x0FFF, vrm, mirror);
    case AddrRangePBus::RG_3F00:
        return read_ppu_vram((addr - 0x1000) & 0x0FFF, vrm, mirror);
    case AddrRangePBus::RG_3F20:
        return read_ppu_pal(addr & 0x001F, pal);
    case AddrRangePBus::RG_4000:
        return read_ppu_pal(addr & 0x001F, pal);
    default:
        return 0;
    }
}

void Disk::WritePBus(const uint16_t &addr, const Byte &data) {
    AddrRangePBus rg = addr_range_ppu(addr);
    switch (rg) {
    case AddrRangePBus::RG_1000:
        chr[addr] = data;
        break;
    case AddrRangePBus::RG_2000:
        chr[addr] = data;
        break;
    case AddrRangePBus::RG_3000:
        write_ppu_vram(addr & 0x0FFF, data, vrm, mirror);
        break;
    case AddrRangePBus::RG_3F00:
        write_ppu_vram((addr - 0x1000) & 0x0FFF, data, vrm, mirror);
        break;
    case AddrRangePBus::RG_3F20:
        write_ppu_pal(addr & 0x001F, data, pal);
        break;
    case AddrRangePBus::RG_4000:
        write_ppu_pal(addr & 0x001F, data, pal);
        break;
    default:
        break;
    }
}
