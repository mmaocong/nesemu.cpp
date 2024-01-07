// ============================================================================
// PPU
//
// References:
//
// - https://www.nesdev.org/wiki/PPU_registers
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>

#include "const.hpp"
#include "disk.hpp"

struct PPU {

    sf::Image image;
    Disk *disk;

    RegW scanline;
    RegW cycle;

    // background 16-bit shift registers:
    //
    // - contain the pattern table data for two tiles
    // - every 8 cycles:
    //   - the data for the next tile is loaded into the upper 8-bit
    //   - the pixel to render is fetched from one of the lower 8-bit
    RegW bg_shift_pat_lo;
    RegW bg_shift_pat_hi;

    // background 8-bit shift registers:
    //
    // - contain the palette attributes for the lower 8 pixels of the 16-bit
    //   shift register.
    // - fed by a latch which contains the palette attribute for the next tile.
    // - every 8 cycles
    //   - the latch is loaded with the palette attribute for the next tile
    RegB bg_shift_attr_lo;
    RegB bg_shift_attr_hi;

    // next background tile id
    RegB bg_tile_id;
    // next background tile attribute
    RegB bg_tile_attr;
    // next background tile pattern (low 8-bit)
    RegB bg_tile_lo;
    // next background tile pattern (high 8-bit)
    RegB bg_tile_hi;

    bool nmi;
    bool frame_complete;

    // Constructor & Destructor
    PPU();
    ~PPU();

    // ------------------------------------------------------------------------
    // functions
    // ------------------------------------------------------------------------

    void PrintNT();
    void PrintV();
    void PrintTile();
    void PrintXY();

    void Reset();

    // attach memory to the CPU
    void Mount(const Disk &);

    // ---------- execute functions ----------

    void RunCycle();

    // ------------------------------------------------------------------------
    // helper functions
    // ------------------------------------------------------------------------

  private:
    // update the background shift registers
    inline void update_bg_shift() {
        if (disk->pram.mask.bg) {
            bg_shift_pat_hi <<= 1;
            bg_shift_pat_lo <<= 1;
            bg_shift_attr_hi <<= 1;
            bg_shift_attr_lo <<= 1;
        }
    }

    // Load the current background tile pattern and attributes into
    // the shift registers
    inline void load_bg_shift() {
        // Each PPU update we calculate one pixel. These shifters shift 1
        // bit along feeding the pixel compositor with the binary
        // information it needs. Its 16 bits wide, because the top 8 bits
        // are the current 8 pixels being drawn and the bottom 8 bits are
        // the next 8 pixels to be drawn. Naturally this means the required
        // bit is always the MSB of the shifter. However, "fine x" scrolling
        // plays a part in this too, whcih is seen later, so in fact we can
        // choose any one of the top 8 bits.
        bg_shift_pat_lo = (bg_shift_pat_lo & 0xFF00) | bg_tile_lo;
        bg_shift_pat_hi = (bg_shift_pat_hi & 0xFF00) | bg_tile_hi;

        // Attribute bits do not change per pixel, rather they change every
        // 8 pixels but are synchronised with the pattern shifters for
        // convenience, so here we take the bottom 2 bits of the attribute
        // word which represent which palette is being used for the current
        // 8 pixels and the next 8 pixels, and "inflate" them to 8 bit
        // words.
        bg_shift_attr_lo =
            (bg_shift_attr_lo & 0xFF00) | ((bg_tile_attr & 0b01) ? 0xFF : 0x00);
        bg_shift_attr_hi =
            (bg_shift_attr_hi & 0xFF00) | ((bg_tile_attr & 0b10) ? 0xFF : 0x00);
    };

    inline void fetch_bg_nt() {
        // debug
        // bg_tile_id = (rand() % 2) ? 0x3F : 0x30;
        bg_tile_id = disk->ReadPBus(0x2000 | (disk->pram.v.reg & 0x0FFF));
    }

    inline void fetch_bg_at() {
        bg_tile_attr = disk->ReadPBus(
            0x23C0 | (disk->pram.v.nty << 11) | (disk->pram.v.ntx << 10) |
            ((disk->pram.v.y_coarse >> 2) << 3) | (disk->pram.v.x_coarse >> 2));

        if (disk->pram.v.y_coarse & 0x02)
            bg_tile_attr >>= 4;
        if (disk->pram.v.x_coarse & 0x02)
            bg_tile_attr >>= 2;
        bg_tile_attr &= 0x03;
    };

    // DCBA98 76543210
    // ---------------
    // 0HNNNN NNNNPyyy
    // |||||| |||||+++- T: Fine Y offset, the row number within a tile
    // |||||| |||||
    // |||||| ||||+---- P: Bit plane
    // |||||| ||||      0: less significant bit; 1: more significant bit
    // |||||| ||||
    // ||++++-++++----- N: Tile number from name table
    // |+-------------- H: Half of pattern table (0: "left"; 1: "right")
    // +--------------- 0: Pattern table is at $0000-$1FFF
    inline void fetch_bg_tile_lo() {
        bg_tile_lo =
            disk->ReadPBus((disk->pram.ctrl.bgp << 12) +
                           ((uint16_t)bg_tile_id << 4) + disk->pram.v.y);
    }

    inline void fetch_bg_tile_hi() {
        bg_tile_hi =
            disk->ReadPBus((disk->pram.ctrl.bgp << 12) +
                           ((uint16_t)bg_tile_id << 4) + disk->pram.v.y + 8);
    };

    // Increment the background tile "pointer" one tile/column horizontally
    inline void inc_bg_x() {
        if (disk->pram.mask.bg || disk->pram.mask.fg) {
            if (disk->pram.v.x_coarse == 31) {
                disk->pram.v.x_coarse = 0;
                disk->pram.v.ntx = ~disk->pram.v.ntx;
            } else {
                disk->pram.v.x_coarse++;
            }
        }
    };

    // Increment the background tile "pointer" one scanline vertically
    inline void inc_bg_y() {
        if (disk->pram.mask.bg || disk->pram.mask.fg) {
            if (disk->pram.v.y < 7) {
                disk->pram.v.y++;
            } else {
                // Reset fine y offset
                disk->pram.v.y = 0;

                if (disk->pram.v.y_coarse == 29) {
                    // need to swap vertical nametable targets
                    // reset coarse y offset
                    disk->pram.v.y_coarse = 0;
                    // flip the target nametable bit
                    disk->pram.v.nty = ~disk->pram.v.nty;
                } else if (disk->pram.v.y_coarse == 31) {
                    // in case the pointer is in the attribute memory, we
                    // just wrap around the current nametable
                    disk->pram.v.y_coarse = 0;
                } else {
                    // None of the above boundary/wrapping conditions apply
                    // so just increment the coarse y offset
                    disk->pram.v.y_coarse++;
                }
            }
        }
    };

    // Transfer the temporarily stored horizontal nametable access information
    // into the "pointer".
    // NOTE: fine x scrolling is NOT part of the "pointer" addressing
    inline void transfer_x() {
        // only if rendering is enabled
        if (disk->pram.mask.bg || disk->pram.mask.fg) {
            disk->pram.v.ntx = disk->pram.t.ntx;
            disk->pram.v.x_coarse = disk->pram.t.x_coarse;
        }
    };

    // Transfer the temporarily stored vertical nametable access information
    // into the "pointer".
    // NOTE: that fine y scrolling is part of the "pointer" addressing
    inline void transfer_y() {
        // only if rendering is enabled
        if (disk->pram.mask.bg || disk->pram.mask.fg) {
            disk->pram.v.y = disk->pram.t.y;
            disk->pram.v.nty = disk->pram.t.nty;
            disk->pram.v.y_coarse = disk->pram.v.y_coarse;
        }
    };

    // a whole BG fetch loop
    // NOTE: cycle MUST be in [1, 258) or [321, 338) (TODO: check)
    inline void bg_fetch_loop() {
        update_bg_shift();
        switch ((cycle - 1) % 8) {
        case 0:
            load_bg_shift();
            fetch_bg_nt();
            break;
        case 2:
            fetch_bg_at();
            break;
        case 4:
            fetch_bg_tile_lo();
            break;
        case 6:
            fetch_bg_tile_hi();
            break;
        case 7:
            inc_bg_x();
            break;
        }
    };

    // Pre-render (scanline 261)
    inline void pre_render() {
        if (cycle == 1) {
            // - cycle 1
            //   - clear VBlank flag
            //   - clear sprite overflow flag
            //   - clear sprite 0-hit flag
            //   - TODO: Clear Shift registers
            disk->pram.status.vbk = 0;
            disk->pram.status.fgof = 0;
            disk->pram.status.fgzh = 0;
        } else if (cycle == 256) {
            bg_fetch_loop();
            inc_bg_y();
        } else if (cycle == 257) {
            bg_fetch_loop();
            load_bg_shift();
            transfer_x();
        } else if ((cycle >= 1 && cycle < 258) ||
                   (cycle >= 321 && cycle < 338)) {
            bg_fetch_loop();
        } else if (cycle == 338 || cycle == 340) {
            fetch_bg_nt();
        }
        // TODO: integrate above as a mutuall exclusive function
        if (cycle >= 280 && cycle < 305) {
            transfer_y();
        }
    }

    // render init (scanline 0)
    inline void render_init() {
        if (cycle == 0) {
            // "Odd Frame" cycle skip
            cycle = 1;
        } else if (cycle == 256) {
            bg_fetch_loop();
            inc_bg_y();
        } else if (cycle == 257) {
            bg_fetch_loop();
            load_bg_shift();
            transfer_x();
        } else if ((cycle >= 1 && cycle < 258) ||
                   (cycle >= 321 && cycle < 338)) {
            bg_fetch_loop();
        } else if (cycle == 338 || cycle == 340) {
            fetch_bg_nt();
        }
    }

    // render (scanline 1-239)
    inline void render() {
        if (cycle == 256) {
            bg_fetch_loop();
            inc_bg_y();
        } else if (cycle == 257) {
            bg_fetch_loop();
            load_bg_shift();
            transfer_x();
        } else if ((cycle >= 1 && cycle < 258) ||
                   (cycle >= 321 && cycle < 338)) {
            bg_fetch_loop();
        } else if (cycle == 338 || cycle == 340) {
            fetch_bg_nt();
        }
    }

    // vblank (scanline 241)
    inline void vblank() {
        if (cycle == 1) {
            // - cycle 1
            //   - set VBlank flag
            //   - if NMI is enabled, trigger NMI
            disk->pram.status.vbk = 1;
            if (disk->pram.ctrl.nmi) {
                nmi = true;
            }
        }
    }
};
