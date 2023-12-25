#include <iostream>

#include "cassette.hpp"
#include "const.hpp"
#include "cpu.hpp"
#include "disk.hpp"

int main() {

    Mem mem = Mem(CPU_MEMSIZE);

    std::cout << "Size of mem: " << mem.size() << std::endl;
    std::cout << "mem[0x200] = " << (int)mem[0x200] << std::endl;
    mem[0x200] = 0x42;
    std::cout << "mem[0x200] = " << (int)mem[0x200] << std::endl;

    Disk disk = Disk();

    std::cout << "PRG ROM SIZE: " << disk.prg.size() << std::endl;
    std::cout << "CHR ROM SIZE: " << disk.chr.size() << std::endl;

    CHeader head = Cassette::Load("./data/nestest.nes", disk.prg, disk.chr);

    std::cout << "address of PRG ROM: " << &disk.prg << std::endl;
    std::cout << "address of CHR ROM: " << &disk.chr << std::endl;

    std::cout << "PRG ROM SIZE: " << disk.prg.size() << std::endl;
    std::cout << "CHR ROM SIZE: " << disk.chr.size() << std::endl;

    std::cout << "address of PRG ROM: " << &disk.prg << std::endl;
    std::cout << "address of CHR ROM: " << &disk.chr << std::endl;

    return 0;
}
