// ============================================================================
// PPU
//
// References:
//
// - https://www.nesdev.org/wiki/PPU_registers
// ============================================================================

#pragma once

#include <cstdint>

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
//   |            (0: read backdrop from EXT pins; 1: output color on EXT pins)
//   |
//   +----------- NMI: Generate an NMI at the start of the vertical blanking
//                interval (0: off; 1: on)
//
// ----------------------------------------------------------------------------

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
