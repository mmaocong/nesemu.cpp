#include <iostream>

#include "misc.hpp"
#include "ppu.hpp"

static constexpr uint16_t kW = 256;
static constexpr uint16_t kH = 240;

// Function to set a pixel to a specific color in an sf::Image
static void set_pixel(sf::Image &image, const uint16_t x, const uint16_t y,
                      uint8_t ind) {
    if (x < image.getSize().x && y < image.getSize().y) {
        image.setPixel(x, y, sf::Color(PAL_MASTER[ind]));
    }
}

// ScanLine State, as per the PPU's scanline ranges
enum class SLState : uint8_t {
    // Any state outside the defined rendering process
    IDLE = 0,
    // Pre-render: traditionally line == -1, adjusted to 261 for zero-indexing
    PRE,
    // first line of visible rendering (0)
    INIT,
    // Visible scanlines (1-239)
    RENDER,
    // Scanline 240, where no rendering occurs
    POST,
    // Vertical blanking lines (241)
    VB,
    // post-VB lines (242-260), where no rendering occurs
    VBPOST,
};

// Function to determine the current PPU render state.
static inline SLState GetSLState(const uint16_t &sl) {
    if (sl == 261) {
        // further refine as per the PPU's cycle timings
        return SLState::PRE;
    } else if (sl == 0) {
        // further refine as per the PPU's cycle timings
        return SLState::INIT;
    } else if (sl >= 1 && sl < 240) {
        // further refine as per the PPU's cycle timings
        return SLState::RENDER;
    } else if (sl == 240) {
        // Post-render line, no actual rendering
        return SLState::POST;
    } else if (sl == 241) {
        // Vertical blanking line
        return SLState::VB;
    } else if (sl >= 242 && sl <= 260) {
        // Post-VB lines, no rendering
        return SLState::VBPOST;
    } else {
        // Any other state is considered Idle
        return SLState::IDLE;
    }
}

// Constructor
PPU::PPU() {
    // Initialize registers
    scanline = cycle = 0;
    bg_shift_pat_lo = bg_shift_pat_hi = 0;
    bg_shift_attr_hi = bg_shift_attr_lo = 0;
    bg_tile_id = bg_tile_attr = bg_tile_lo = bg_tile_hi = 0;
    nmi = false;
    frame_complete = false;

    // Initialize memory to nullptr
    disk = nullptr;
    image.create(kW, kH, sf::Color::Black);
}

void PPU::Reset() {
    // Initialize registers
    scanline = cycle = 0;
    bg_shift_pat_lo = bg_shift_pat_hi = 0;
    bg_shift_attr_hi = bg_shift_attr_lo = 0;
    bg_tile_id = bg_tile_attr = bg_tile_lo = bg_tile_hi = 0;
    nmi = false;

    image.create(kW, kH, sf::Color::Black);
}

// Destructor
PPU::~PPU() {}

// TODO: shared_ptr
void PPU::Mount(const Disk &disk) { this->disk = (Disk *)&disk; }

void PPU::RunCycle() {
    SLState state = GetSLState(scanline);
    switch (state) {
    case SLState::PRE:
        pre_render();
    case SLState::INIT:
        render_init();
    case SLState::RENDER:
        render();
    case SLState::VB:
        vblank();
    default:
        break;
    }

    // The 2-bit pixel to be rendered
    uint8_t bg_pixel = 0x00;
    // The 3-bit index of the palette the pixel indexes
    uint8_t bg_palette = 0x00;

    // We only render backgrounds if the PPU is enabled to do so. Note if
    // background rendering is disabled, the pixel and palette combine
    // to form 0x00. This will fall through the colour tables to yield
    // the current background colour in effect
    if (disk->pram.mask.bg || !disk->pram.mask.bg) {
        // Handle Pixel Selection by selecting the relevant bit
        // depending upon fine x scolling. This has the effect of
        // offsetting ALL background rendering by a set number
        // of pixels, permitting smooth scrolling
        uint16_t bit_mux = 0x8000 >> disk->pram.x;

        // Select Plane pixels by extracting from the shifter
        // at the required location.
        uint8_t p0_pixel = (bg_shift_pat_lo & bit_mux) > 0;
        uint8_t p1_pixel = (bg_shift_pat_hi & bit_mux) > 0;

        // Combine to form pixel index
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        // Get palette
        uint8_t bg_pal0 = (bg_shift_attr_lo & bit_mux) > 0;
        uint8_t bg_pal1 = (bg_shift_attr_hi & bit_mux) > 0;
        bg_palette = (bg_pal1 << 1) | bg_pal0;
    }

    uint8_t color =
        disk->ReadPBus(0x3F00 + (bg_palette << 2) + bg_pixel) & 0x3F;
    // std::cout << Misc::hex(color, 2) << "; Palette:" << Misc::hex(bg_palette, 2)
    //           << "; Pixel:" << Misc::hex(bg_pixel, 2) << std::endl;
    set_pixel(image, cycle - 1, scanline, color);
    // fake rendering
    // set_pixel(image, cycle - 1, scanline, (rand() % 2) ? 0x3F : 0x30);

    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;
        if (scanline > 261) {
            scanline = 0;
            frame_complete = true;
        }
    }
}
