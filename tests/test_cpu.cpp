#include <gtest/gtest.h>

#include "cassette.hpp"
#include "cpu.hpp"

// Test CPU by running nestest.nes and comparing the registers and cycles.
//
// Log Reference: https://www.qmtpro.com/~nes/misc/nestest.log
//
// Note: currently only covers the top ~5000 instructions as the program
//       contains many illegal opcodes that are not implemented
TEST(CPUTest, NesLogCompare) {

    Mem mem = Mem(CPU_MEMSIZE);
    CPU cpu = CPU();

    Disk disk = Disk();

    CHeader head = Cassette::Load("./data/nestest.nes", disk.prg, disk.chr);

    cpu.Mount(disk);

    cpu.Reset();
    cpu.PC = 0xC000;
    int limit = 5250;

    size_t cyc_count = 0;
    for (int i = 1; i < limit; i++) {
        cyc_count += cpu.RunInstr();
    }

    EXPECT_EQ(cpu.RA, 0x32);
    EXPECT_EQ(cpu.RX, 0x00);
    EXPECT_EQ(cpu.RY, 0x58);
    EXPECT_EQ(cpu.RF.reg, 0x25);
    EXPECT_EQ(cpu.SP, 0xFB);
    EXPECT_EQ(cyc_count, 15252);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}