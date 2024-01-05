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

// PPU Registers (0x2000 - 0x2007, 0x4014, loopy's).
//
// References:
//
// - https://www.nesdev.org/wiki/PPU_registers
struct PMem {

    union LRegW {
        struct {
            uint16_t x_coarse : 5;
            uint16_t y_coarse : 5;
            uint16_t ntx : 1;
            uint16_t nty : 1;
            uint16_t y : 3; // Fine Y scroll
            uint16_t unused : 1;
        };
        RegW reg;
    };

    // ------------------------------------------------------------------------
    // Register: PPUCTRL (0x2000) > write
    //
    //   7  bit  0
    //   ---- ----
    //   VPHB SINN
    //   |||| ||||
    //   |||| ||++--- NTX & NTY: Base nametable address
    //   |||| ||      (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
    //   |||| ||
    //   |||| |+----- VRAM address increment per CPU read/write of PPUDATA
    //   |||| |       (0: add 1, going across; 1: add 32, going down)
    //   |||| |
    //   |||| +------ Sprite pattern table address for 8x8 sprites
    //   ||||         (0: $0000; 1: $1000; ignored in 8x16 mode)
    //   ||||
    //   |||+-------- Background pattern table address (0: $0000; 1: $1000)
    //   |||
    //   ||+--------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
    //   ||
    //   |+---------- PPU master/slave select
    //   |            (0: read backdrop from EXT pins; 1: output color on EXT
    //   pins)
    //   |
    //   +----------- NMI: Generate an NMI at the start of the vertical blanking
    //                interval (0: off; 1: on)
    //
    // ------------------------------------------------------------------------
    union {
        RegB reg;
        struct {
            // Name table X
            uint8_t ntx : 1;
            // Name table Y
            uint8_t nty : 1;
            // VRAM address increment mode: 0: add 1; 1: add 32
            uint8_t inc : 1;
            // Sprite pattern table address for 8x8 sprites:
            // 0: $0000; 1: $1000; ignored in 8x16 mode
            uint8_t stb : 1;
            // Background pattern table address: 0: $0000; 1: $1000
            uint8_t btb : 1;
            // Sprite size: 0: 8x8 pixels; 1: 8x16 pixels
            uint8_t ssz : 1;
            // PPU master/slave select:
            // 0: read backdrop from EXT pins; 1: output
            uint8_t slave : 1;
            // Generate an NMI at the start of the vertical blanking interval:
            // 0: off; 1: on
            uint8_t nmi : 1;
        };
    } ctrl;

    // ------------------------------------------------------------------------
    // Register: PPUMASK (0x2001) > write
    //
    //   7  bit  0
    //   ---- ----
    //   BGRs bMmG
    //   |||| ||||
    //   |||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
    //   |||| |||
    //   |||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
    //   |||| ||
    //   |||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
    //   |||| |
    //   |||| +---- 1: Show background
    //   ||||
    //   |||+------ 1: Show sprites
    //   |||
    //   ||+------- Emphasize red (green on PAL/Dendy)
    //   ||
    //   |+-------- Emphasize green (red on PAL/Dendy)
    //   |
    //   +--------- Emphasize blue
    // ------------------------------------------------------------------------
    union {
        RegB reg;
        struct {
            // Greyscale
            uint8_t gray : 1;
            // Show background in leftmost 8 pixels of screen
            uint8_t bgl : 1;
            // Show sprites in leftmost 8 pixels of screen
            uint8_t fgl : 1;
            // Show background
            uint8_t bgs : 1;
            // Show sprites
            uint8_t fgs : 1;
            // Emphasize red (green on PAL/Dendy)
            uint8_t red : 1;
            // Emphasize green (red on PAL/Dendy)
            uint8_t grn : 1;
            // Emphasize blue
            uint8_t blu : 1;
        };
    } mask;

    // ------------------------------------------------------------------------
    // Register: PPUSTATUS (0x2002) < read
    //
    //   7  bit  0
    //   ---- ----
    //   VSO. ....
    //   |||| ||||
    //   |||+-++++- PPU open bus. Returns stale PPU bus contents.
    //   |||
    //   ||+------- Sprite overflow.
    //   ||         - intented to be set whenever more than eight sprites
    //   ||           appear on a scanline
    //   ||         - hardware bug causes false positives as well as false
    //   ||           negatives
    //   ||         - it is set during sprite evaluation and cleared at dot 1
    //   ||           (the second dot) of the pre-render line.
    //   ||
    //   |+-------- Sprite 0 Hit.
    //   |          - set when a nonzero pixel of sprite 0 overlaps a nonzero
    //   |            background pixel
    //   |          - cleared at dot 1 of the pre-render line
    //   |          - Used for raster timing.
    //   |
    //   +--------- Vertical blank has started.
    //              (0: not in vblank; 1: in vblank).
    //              Set at dot 1 of line 241 (the line *after* the post-render
    //              line); cleared after reading $2002 and at dot 1 of the
    //              pre-render line.
    // ------------------------------------------------------------------------
    union {
        RegB reg;
        struct {
            // PPU open bus. Returns stale PPU bus contents.
            uint8_t unused : 5;
            // Sprite overflow
            uint8_t fgof : 1;
            // Sprite Zero Hit
            uint8_t fgzh : 1;
            // Vertical blank has started
            uint8_t vbk : 1;
        };
    } status;

    // ------------------------------------------------------------------------
    // OAMADDR: OAM Address Register (0x2003) > write
    //
    //   - 7  bit  0
    //   - ---- ----
    //   - aaaa aaaa
    //   - |||| ||||
    //   - +++++++++- OAM read/write address
    //
    // ------------------------------------------------------------------------
    RegB oamaddr;

    // ------------------------------------------------------------------------
    // OAMDATA: OAM Data Register (0x2004) <> read/write.
    //
    //   - 7  bit  0
    //   - ---- ----
    //   - dddd dddd
    //   - |||| ||||
    //   - +++++++++- OAM data read/write.
    //
    // ------------------------------------------------------------------------
    RegB oamdata;

    // ------------------------------------------------------------------------
    // PPUSCROLL: PPU scrolling position (0x2005) >> write x2
    //
    //   - 7  bit  0
    //   - ---- ----
    //   - dddd dddd
    //   - |||| ||||
    //   - +++++++++- Fine scroll position (two writes: 1st X, 2nd Y)
    //
    // - Whether it is the 1st or 2nd write is determined by a latch, i.e.
    //   internal W register, which is shared with PPUADDR.
    // ------------------------------------------------------------------------
    RegB ppuscroll;

    // ------------------------------------------------------------------------
    // PPUADDR: PPU address register (0x2006) >> write x2
    //
    //   - 7  bit  0
    //   - ---- ----
    //   - aaaa aaaa
    //   - |||| ||||
    //   - +++++++++- PPU read/write address (two writes: 1st MSB, 2nd LSB)
    //
    // - shared with PPUSCROLL the latch (internal W register)
    //
    // ------------------------------------------------------------------------
    RegB ppuaddr;

    // ------------------------------------------------------------------------
    // PPUDATA: PPU data register (0x2007) <> read/write
    //
    //   - 7  bit  0
    //   - ---- ----
    //   - dddd dddd
    //   - |||| ||||
    //   - +++++++++- PPU data read/write
    //
    // ------------------------------------------------------------------------
    RegB ppudata;

    // ------------------------------------------------------------------------
    // OAMDMA: OAM DMA register (0x4014) > write
    //
    //   - 7  bit  0
    //   - ---- ----
    //   - aaaa aaaa
    //   - |||| ||||
    //   - +++++++++- OAM DMA high address
    //
    // ------------------------------------------------------------------------
    RegB oamdma;

    // Active address into NT to extract BG tile
    LRegW v;
    // Temporary store to be transferred to lrv from time to time
    LRegW t;
    RegB w;      // Internal latch
    RegB x;      // Fine X scroll (3 bits)
    RegB buffer; // Buffer when reading PPU registers

    // Get the address increment based on the increment mode.
    // - Vertical mode: +32, which skips one whole nametable row
    // - Horizontal mode: +1, moving to the next column
    inline uint16_t AddrInc() { return ctrl.inc ? 32 : 1; }
};

// Collection of Memory
struct Disk {

    // ------------------------------------------------------------------------
    // Array-like storage
    // ------------------------------------------------------------------------

    Mem ram; // 8KB internal RAM + 8KB PPU registers + 32B APU & I/O registers
    Mem vrm; // NT * 4 + AT * 4 (4KB)
    Mem pal; // palette (32B)
    Mem prg; // max: 32KB. TODO: expand
    Mem chr; // max: 8KB. TODO: expand

    // ------------------------------------------------------------------------
    // PPU related
    //
    // NOTE: They are not a part of the memory, but it is included here because
    //       reading / writing to 0x2000 - 0x2007 of the main RAM, i.e. PPU
    //       registers, will affect the values of these variables.
    //       They will interact with `ReadCPU` and `WriteCPU` functions.
    // ------------------------------------------------------------------------

    // PPU registers
    PMem pram;

    // ------------------------------------------------------------------------
    // Cartridge related
    // ------------------------------------------------------------------------

    uint16_t prg_kb;
    uint16_t chr_kb;
    MirrorMode mirror;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    // ---------- Constructor & Destructor ----------

    Disk();
    ~Disk();

    // print disk info
    void Print();

    // Attach a cartridge
    void Attach(const std::string &);

    // ---------- Read / Write via Main Bus ----------

    // Read 1 byte via the main bus
    Byte ReadMBus(const uint16_t &);

    // Write 1 byte via the main bus
    void WriteMBus(const uint16_t &, const Byte &);

    Byte ReadPRam(const uint16_t &);

    void WritePRam(const uint16_t &, const Byte &);

    // ---------- Read / Write via the PPU Bus ----------

    // Read 1 byte via the PPU bus
    Byte ReadPBus(const uint16_t &);

    // Write 1 byte via the PPU bus
    void WritePBus(const uint16_t &, const Byte &);
};
