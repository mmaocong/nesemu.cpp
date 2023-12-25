#include <fstream>
#include <iostream>

#include "misc.hpp"
#include "neshdr.hpp"

// Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file)
static constexpr char NES_NAME[4] = {0x4E, 0x45, 0x53, 0x1A};

void NesHdr::Print() const {
    std::cout << name[0] << name[1] << name[2] << ":" << std::endl;
    std::cout << "PRG size: " << +n_chunk_prg << std::endl;
    std::cout << "CHR size: " << +n_chunk_chr << std::endl;
    // flags 6
    std::cout << "Vertical-Mirror:" << bool(vertical)
              << " Battery:" << bool(battery) << " Trainer:" << bool(trainer)
              << " Four-Screen:" << bool(four) << std::endl;
    // flags 7
    std::cout << "VS-Unisystem:" << bool(vs) << " Playchoice10:" << bool(pc10)
              << " INES-2.0:" << bool(nes2) << std::endl;
    // mapper
    std::cout << "Mapper:" << Misc::hex(mapper_hi, 1) << Misc::hex(mapper_lo, 1)
              << std::endl;
    std::cout << "PAL:" << bool(pal) << std::endl;
}
