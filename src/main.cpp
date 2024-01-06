#include <iostream>

#include "const.hpp"
#include "misc.hpp"
#include "nes.hpp"

int main() {

    NES nes;

    nes.Load("./data/nestest.nes");
    nes.cpu.PC = 0xC000;

    // // smoke test
    // nes.Run();

    // nes.disk->WritePBus(0x2006, 0x20);
    // nes.disk->WritePBus(0x2007, 0xAB);

    // // nametable check
    // static constexpr uint16_t k1 = 0x2000;
    // static constexpr uint16_t k2 = 0x23C0;
    // for (uint16_t i = k1; i < k2; i++) {
    //     Byte data = nes.disk->ReadPBus(i);
    //     if (i % 32 == 0) {
    //         std::cout << std::endl;
    //     }
    //     std::cout << Misc::hex(data, 2) << " ";
    // }
    // for (uint16_t i = k1; i < k2; i++) {
    //     Byte data = (rand() % 2) ? 0x3F : 0x30;
    //     nes.disk->WritePBus(i, data);
    // }
    // for (uint64_t i = 0; i < 100; i++) {
    //     nes.RunCycle();
    // }
    // for (uint16_t i = k1; i < k2; i++) {
    //     Byte data = nes.disk->ReadPBus(i);
    //     if (i % 32 == 0) {
    //         std::cout << std::endl;
    //     }
    //     std::cout << Misc::hex(data, 2) << " ";
    // }

    // // Bin to asm
    // std::vector<std::string> asm_code;
    // nes.cpu.BinToAsm(nes.disk->prg, asm_code);
    // for (int i = 0; i < asm_code.size(); i++) {
    //     std::cout << asm_code[i] << std::endl;
    // }

    // // check log
    // for (size_t i = 0; i < 100; i++) {
    //     nes.cpu.RunCycle();
    //     nes.cpu.Print();
    //     std::cout << "X:" << +nes.ppu.cycle << " Y:" << +nes.ppu.scanline
    //               << std::endl;
    // }

    return 0;
}
