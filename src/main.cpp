#include <iostream>

#include "const.hpp"
#include "cpu.hpp"
#include "disk.hpp"
#include "neshdr.hpp"

int main() {

    Disk disk = Disk();
    disk.Attach("./data/nestest.nes");
    disk.Print();

    return 0;
}
