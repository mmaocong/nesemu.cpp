#pragma once

#include "cpu.hpp"
#include "disk.hpp"
#include "ppu.hpp"
#include <SFML/Graphics.hpp>

struct NES {
    CPU cpu;
    PPU ppu;
    std::shared_ptr<Disk> disk;
    sf::RenderWindow window;
    size_t cycles;

    // Constructor
    NES();
    // Destructor
    ~NES();

    void Load(const std::string &);
    void RunCycle();

    void Run();
};
