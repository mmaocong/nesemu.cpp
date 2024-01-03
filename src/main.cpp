#include <iostream>

#include "cassette.hpp"
#include "const.hpp"
#include "cpu.hpp"
#include "disk.hpp"

int main() {

    Mem mem = Mem(CPU_MEMSIZE);
    CPU cpu = CPU();
    std::vector<std::string> asmcode;

    Disk disk = Disk();

    std::cout << "PRG ROM SIZE: " << disk.prg.size() << std::endl;
    std::cout << "CHR ROM SIZE: " << disk.chr.size() << std::endl;

    CHeader head = Cassette::Load("./data/nestest.nes", disk.prg, disk.chr);

    cpu.Mount(disk);

    cpu.Reset();
    // cpu.IRQ();
    // std::cout << "PC: " << std::hex << +cpu.PC << std::endl;
    cpu.Exec(0xC000, 5250);

    // std::cout << "PRG ROM SIZE: " << disk.prg.size() << std::endl;

    // cpu.BinToAsm(disk.prg, asmcode);
    // // top 100
    // for (int i = 0; i < 1000; i++) {
    //     std::cout << asmcode[i] << std::endl;
    // }

    // std::cout << std::hex << (int)disk.prg[0x0000] << std::endl;
    // std::cout << std::hex << (int)disk.prg[0x0001] << std::endl;
    // std::cout << std::hex << (int)disk.prg[0x0002] << std::endl;

    // size_t s = 0;
    // for (auto i : disk.prg) {
    //     std::cout << std::hex << (int)i << std::endl;
    //     s++;
    // }
    // std::cout << "PRG ROM SIZE: " << s << std::endl;

    // std::cout << "address of PRG ROM: " << &disk.prg << std::endl;
    // std::cout << "address of CHR ROM: " << &disk.chr << std::endl;

    // std::cout << "PRG ROM SIZE: " << disk.prg.size() << std::endl;
    // std::cout << "CHR ROM SIZE: " << disk.chr.size() << std::endl;

    // std::cout << "address of PRG ROM: " << &disk.prg << std::endl;
    // std::cout << "address of CHR ROM: " << &disk.chr << std::endl;

    return 0;
}
