#include "disk.hpp"
#include "const.hpp"
#include "neshdr.hpp"

#include <fstream>
#include <iostream>

// Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file)
static constexpr char NES_NAME[4] = {0x4E, 0x45, 0x53, 0x1A};

// ----------------------------------------------------------------------------
// Disk Class
// ----------------------------------------------------------------------------

// Constructor
Disk::Disk()
    : ram(kRAMSize, 0), vrm(kVRAMSize), pal(kPaletteSize), prg(0), chr(0) {}

// Destructor
Disk::~Disk() {}

void Disk::Print() {
    std::cout << "PRG-ROM:" << prg_kb << "KB; CHR-ROM:" << chr_kb << "KB"
              << std::endl;
    std::cout << "Mirroring:";
    switch (mirror) {
    case MirrorMode::HORIZ:
        std::cout << "Horizontal" << std::endl;
        break;
    case MirrorMode::VERT:
        std::cout << "Vertical" << std::endl;
        break;
    case MirrorMode::FOUR:
        std::cout << "Four" << std::endl;
        break;
    default:
        std::cout << "Unknown" << std::endl;
    }
}

void Disk::Attach(const std::string &cart) {

    std::ifstream file(cart, std::ios::binary);

    NesHdr header;

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + cart);
    }

    if (!file.read((char *)&header, sizeof(NesHdr)) ||
        std::memcmp(header.name, NES_NAME, 4) != 0) {
        throw std::runtime_error("Failed to read header");
    }

    // If a "trainer" exists we just need to read past it before we get to the
    // good stuff
    if (header.trainer)
        file.seekg(512, std::ios_base::cur);

    // determine mirroring mode
    if (header.four) {
        mirror = MirrorMode::FOUR;
    } else if (header.vertical) {
        mirror = MirrorMode::VERT;
    } else {
        mirror = MirrorMode::HORIZ;
    }

    // update PRG/CHR size
    prg_kb = header.n_chunk_prg * 16; // 16kb chunks
    chr_kb = header.n_chunk_chr * 8;  // 8kb chunks

    // restrict for now:
    // - 32KB PRG-ROM
    // -  8KB CHR-ROM
    if (prg_kb > 32) {
        throw std::runtime_error(
            "Unsupported PRG-ROM size: " + std::to_string(prg_kb) + "KB");
    }
    if (chr_kb > 8) {
        throw std::runtime_error(
            "Unsupported CHR-ROM size: " + std::to_string(chr_kb) + "KB");
    }

    // read prg rom
    prg.resize(prg_kb * 1024);
    chr.resize(chr_kb * 1024);

    if (!file.read((char *)prg.data(), prg.size())) {
        throw std::runtime_error("Failed to read prg rom");
    }
    if (!file.read((char *)chr.data(), chr.size())) {
        throw std::runtime_error("Failed to read chr rom");
    }
}
