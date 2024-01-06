#include "nes.hpp"

static constexpr uint16_t kW = 256;
static constexpr uint16_t kH = 240;

// Constructor & Destructor
NES::NES() {
    cpu = CPU();
    ppu = PPU();
    disk = std::make_shared<Disk>();
    cycles = 0;
    window.create(sf::VideoMode(kW, kH), "MyNES",
                  sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
}
NES::~NES() {}

void NES::Load(const std::string &file) {
    disk->Attach(file);
    cpu.Mount(*disk);
    ppu.Mount(*disk);
    cpu.Reset();
    ppu.Reset();
}

void NES::RunCycle() {
    ppu.RunCycle();
    if (cycles % 3 == 0) {
        cpu.RunCycle();
    }
    if (ppu.nmi) {
        ppu.nmi = false;
        cpu.NMI();
    }
    cycles++;
}

void NES::Run() {
    // Load the image into a texture
    sf::Texture texture;
    // Create a sprite that we can draw onto the screen
    sf::Sprite sprite;

    // for (size_t i = 0; i < 10000; i++)
    //     RunCycle();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            } else if (event.type == sf::Event::GainedFocus) {
                cpu.Reset();
                ppu.Reset();
            }
        }

        // Clock enough times to draw a single frame
        do {
            RunCycle();
        } while (!ppu.frame_complete);
        // Use residual clock cycles to complete current instruction
        do {
            RunCycle();
        } while (!(cpu.cycles == 0));
        // Reset frame completion flag
        ppu.frame_complete = false;

        // Convert the updated PPU image to texture and apply to sprite
        texture.loadFromImage(ppu.image);
        sprite.setTexture(texture);

        // Clear the window and draw the sprite
        window.clear();
        window.draw(sprite);

        // Update the window
        window.display();
    }
}
