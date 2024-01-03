// ============================================================================
// PPU
//
// References:
//
// - https://www.nesdev.org/wiki/PPU_registers
// ============================================================================

#pragma once

#include <cstdint>

struct PPU {

    // ----------------------------------------------------------------------------
    // Register: PPUCTRL (0x2000)
    //
    //   7  bit  0
    //   ---- ----
    //   VPHB SINN
    //   |||| ||||
    //   |||| ||++--- NTX & NTY: Base nametable address
    //   |||| ||      (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
    //   |||| ||
    //   |||| |+----- INC: VRAM address increment per CPU read/write of PPUDATA
    //   |||| |       (0: add 1, going across; 1: add 32, going down)
    //   |||| |
    //   |||| +------ STB: Sprite pattern table address for 8x8 sprites
    //   ||||         (0: $0000; 1: $1000; ignored in 8x16 mode)
    //   ||||
    //   |||+-------- BTB: Background pattern table address (0: $0000; 1: $1000)
    //   |||
    //   ||+--------- SSZ: Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
    //   ||
    //   |+---------- MOS: PPU master/slave select
    //   |            (0: read backdrop from EXT pins; 1: output color on EXT
    //   pins)
    //   |
    //   +----------- NMI: Generate an NMI at the start of the vertical blanking
    //                interval (0: off; 1: on)
    //
    // ----------------------------------------------------------------------------
    union RegCtrl {
        uint8_t reg;
        struct {
            // Generate an NMI at the start of the vertical blanking interval:
            // 0: off; 1: on
            uint8_t nmi : 1;
            // PPU master/slave select:
            // 0: read backdrop from EXT pins; 1: output
            uint8_t mos : 1;
            // Sprite size: 0: 8x8 pixels; 1: 8x16 pixels
            uint8_t ssz : 1;
            // Background pattern table address: 0: $0000; 1: $1000
            uint8_t btb : 1;
            // Sprite pattern table address for 8x8 sprites:
            // 0: $0000; 1: $1000; ignored in 8x16 mode
            uint8_t stb : 1;
            // VRAM address increment mode: 0: add 1; 1: add 32
            uint8_t inc : 1;
            // Name table Y
            uint8_t nty : 1;
            // Name table X
            uint8_t ntx : 1;
        };
    };

    // ----------------------------------------------------------------------------
    // Register: PPUMASK (0x2001)
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
    // ----------------------------------------------------------------------------
    union RegMask {
        uint8_t reg;
        struct {
            // Emphasize blue
            uint8_t ebl : 1;
            // Emphasize green (red on PAL/Dendy)
            uint8_t egr : 1;
            // Emphasize red (green on PAL/Dendy)
            uint8_t erd : 1;
            // Show sprites
            uint8_t fgs : 1;
            // Show background
            uint8_t bgs : 1;
            // Show sprites in leftmost 8 pixels of screen
            uint8_t fgl : 1;
            // Show background in leftmost 8 pixels of screen
            uint8_t bgl : 1;
            // Greyscale
            uint8_t gry : 1;
        };
    };

    // ----------------------------------------------------------------------------
    // Register: PPUSTATUS (0x2002)
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
    // ----------------------------------------------------------------------------
    union RegStatus {
        uint8_t reg;
        struct {
            // PPU open bus. Returns stale PPU bus contents.
            uint8_t unused : 5;
            // Sprite overflow
            uint8_t sov : 1;
            // Sprite Zero Hit
            uint8_t szh : 1;
            // Vertical blank has started
            uint8_t vbk : 1;
        };
    };

};
