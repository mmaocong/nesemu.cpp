#include <iostream>
#include <string>

#include "cpu.hpp"
#include "misc.hpp"

// map addressing mode to string name
// unofficial modes will have the standard names
static const std::vector<std::string> map_str_addrmode = {
    "UNK", "IMP", "IMM", "ZPG", "ZPX", "ZPY", "REL", "ABS",
    "ABX", "ABX", "ABY", "ABY", "IND", "IZX", "IZY", "IZY",
};

// map addressing mode to function pointer
static const std::vector<void (CPU::*)(void)> map_func_addrmode = {
    &CPU::XXX, &CPU::IMP, &CPU::IMM, &CPU::ZPG, &CPU::ZPX, &CPU::ZPY,
    &CPU::REL, &CPU::ABS, &CPU::ABX, &CPU::AXP, &CPU::ABY, &CPU::AYP,
    &CPU::IND, &CPU::IZX, &CPU::IZY, &CPU::IYP,
};

// map addressing mode to number of addresses to read.
// This is to indicate whether the operation is nullary, unary, or binary.
static const std::vector<uint8_t> map_addrs = {
    0, // UNK
    0, // IMP
    1, // IMM
    1, // ZPG
    1, // ZPX
    1, // ZPY
    1, // REL
    2, // ABS
    2, // ABX
    2, // AXP
    2, // ABY
    2, // AYP
    2, // IND
    1, // IZX
    1, // IZY
    1, // IYP
};

// map instruction to string name
// unofficial instructions will have the standard names
static const std::vector<std::string> map_str_instruct = {
    "UNK", "BRK", "ORA", "ORA", "XXX", "NOP", "ASL", "ASL", "PHP", "CLC", "JSR",
    "BIT", "AND", "AND", "ROL", "ROL", "PLP", "BMI", "SEC", "RTI", "EOR", "EOR",
    "LSR", "LSR", "PHA", "JMP", "BVC", "CLI", "RTS", "ADC", "ADC", "ROR", "ROR",
    "PLA", "BVS", "SEI", "STA", "STY", "STX", "DEY", "TXA", "BCC", "TYA", "TXS",
    "LDY", "LDY", "LDA", "LDA", "LDX", "LDX", "TAY", "TAX", "BCS", "CLV", "TSX",
    "CPY", "CPY", "CMP", "CMP", "DEC", "INY", "DEX", "BNE", "CLD", "CPX", "CPX",
    "SBC", "SBC", "INC", "INX", "BEQ", "SED", "BPL",
};

// map instruction to function pointer
static const std::vector<void (CPU::*)(void)> map_func_instruct = {
    &CPU::XXX, &CPU::BRK, &CPU::ORA, &CPU::ORI, &CPU::XXX, &CPU::NOP, &CPU::ASL,
    &CPU::ALA, &CPU::PHP, &CPU::CLC, &CPU::JSR, &CPU::BIT, &CPU::AND, &CPU::ANI,
    &CPU::ROL, &CPU::RLA, &CPU::PLP, &CPU::BMI, &CPU::SEC, &CPU::RTI, &CPU::EOR,
    &CPU::EOI, &CPU::LSR, &CPU::LRA, &CPU::PHA, &CPU::JMP, &CPU::BVC, &CPU::CLI,
    &CPU::RTS, &CPU::ADC, &CPU::ADI, &CPU::ROR, &CPU::RRA, &CPU::PLA, &CPU::BVS,
    &CPU::SEI, &CPU::STA, &CPU::STY, &CPU::STX, &CPU::DEY, &CPU::TXA, &CPU::BCC,
    &CPU::TYA, &CPU::TXS, &CPU::LDY, &CPU::LYI, &CPU::LDA, &CPU::LAI, &CPU::LDX,
    &CPU::LXI, &CPU::TAY, &CPU::TAX, &CPU::BCS, &CPU::CLV, &CPU::TSX, &CPU::CPY,
    &CPU::CYI, &CPU::CMP, &CPU::CMI, &CPU::DEC, &CPU::INY, &CPU::DEX, &CPU::BNE,
    &CPU::CLD, &CPU::CPX, &CPU::CXI, &CPU::SBC, &CPU::SBI, &CPU::INC, &CPU::INX,
    &CPU::BEQ, &CPU::SED, &CPU::BPL,
};

// operation
// bridge between the 8-bit opcode and the addressing mode + instruct
struct Operation {
    Instruct instruct = Instruct::UNK;
    AddrMode addrmode = AddrMode::UNK;
    uint8_t cycles = 0;
};

// map opcode to corresponding addressing mode and instruction
static const std::vector<Operation> map_op = {
    {Instruct::BRK, AddrMode::IMM, 7}, // 0x00
    {Instruct::ORA, AddrMode::IZX, 6}, // 0x01
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x02
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x03
    {Instruct::NOP, AddrMode::ZPG, 3}, // 0x04
    {Instruct::ORA, AddrMode::ZPG, 3}, // 0x05
    {Instruct::ASL, AddrMode::ZPG, 5}, // 0x06
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x07
    {Instruct::PHP, AddrMode::IMP, 3}, // 0x08
    {Instruct::ORI, AddrMode::IMM, 2}, // 0x09
    {Instruct::ALA, AddrMode::IMP, 2}, // 0x0A
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x0B
    {Instruct::NOP, AddrMode::ABS, 4}, // 0x0C
    {Instruct::ORA, AddrMode::ABS, 4}, // 0x0D
    {Instruct::ASL, AddrMode::ABS, 6}, // 0x0E
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x0F
    {Instruct::BPL, AddrMode::REL, 2}, // 0x10
    {Instruct::ORA, AddrMode::IZY, 5}, // 0x11
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x12
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x13
    {Instruct::NOP, AddrMode::ZPX, 4}, // 0x14
    {Instruct::ORA, AddrMode::ZPX, 4}, // 0x15
    {Instruct::ASL, AddrMode::ZPX, 6}, // 0x16
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x17
    {Instruct::CLC, AddrMode::IMP, 2}, // 0x18
    {Instruct::ORA, AddrMode::ABY, 4}, // 0x19
    {Instruct::NOP, AddrMode::IMP, 2}, // 0x1A
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x1B
    {Instruct::NOP, AddrMode::ABX, 4}, // 0x1C
    {Instruct::ORA, AddrMode::ABX, 4}, // 0x1D
    {Instruct::ASL, AddrMode::AXP, 7}, // 0x1E
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x1F
    {Instruct::JSR, AddrMode::ABS, 6}, // 0x20
    {Instruct::AND, AddrMode::IZX, 6}, // 0x21
    {Instruct::XXX, AddrMode::ZPX, 2}, // 0x22
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x23
    {Instruct::BIT, AddrMode::ZPG, 3}, // 0x24
    {Instruct::AND, AddrMode::ZPG, 3}, // 0x25
    {Instruct::ROL, AddrMode::ZPG, 5}, // 0x26
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x27
    {Instruct::PLP, AddrMode::IMP, 4}, // 0x28
    {Instruct::ANI, AddrMode::IMM, 2}, // 0x29
    {Instruct::RLA, AddrMode::IMP, 2}, // 0x2A
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x2B
    {Instruct::BIT, AddrMode::ABS, 4}, // 0x2C
    {Instruct::AND, AddrMode::ABS, 4}, // 0x2D
    {Instruct::ROL, AddrMode::ABS, 6}, // 0x2E
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x2F
    {Instruct::BMI, AddrMode::REL, 2}, // 0x30
    {Instruct::AND, AddrMode::IZY, 5}, // 0x31
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x32
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x33
    {Instruct::NOP, AddrMode::ZPX, 4}, // 0x34
    {Instruct::AND, AddrMode::ZPX, 4}, // 0x35
    {Instruct::ROL, AddrMode::ZPX, 6}, // 0x36
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x37
    {Instruct::SEC, AddrMode::IMP, 2}, // 0x38
    {Instruct::AND, AddrMode::ABY, 4}, // 0x39
    {Instruct::NOP, AddrMode::IMP, 2}, // 0x3A
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x3B
    {Instruct::NOP, AddrMode::ABX, 4}, // 0x3C
    {Instruct::AND, AddrMode::ABX, 4}, // 0x3D
    {Instruct::ROL, AddrMode::AXP, 7}, // 0x3E
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x3F
    {Instruct::RTI, AddrMode::IMP, 6}, // 0x40
    {Instruct::EOR, AddrMode::IZX, 6}, // 0x41
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x42
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x43
    {Instruct::NOP, AddrMode::ZPG, 3}, // 0x44
    {Instruct::EOR, AddrMode::ZPG, 3}, // 0x45
    {Instruct::LSR, AddrMode::ZPG, 5}, // 0x46
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x47
    {Instruct::PHA, AddrMode::IMP, 3}, // 0x48
    {Instruct::EOI, AddrMode::IMM, 2}, // 0x49
    {Instruct::LRA, AddrMode::IMP, 2}, // 0x4A
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x4B
    {Instruct::JMP, AddrMode::ABS, 3}, // 0x4C
    {Instruct::EOR, AddrMode::ABS, 4}, // 0x4D
    {Instruct::LSR, AddrMode::ABS, 6}, // 0x4E
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x4F
    {Instruct::BVC, AddrMode::REL, 2}, // 0x50
    {Instruct::EOR, AddrMode::IZY, 5}, // 0x51
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x52
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x53
    {Instruct::NOP, AddrMode::ZPX, 4}, // 0x54
    {Instruct::EOR, AddrMode::ZPX, 4}, // 0x55
    {Instruct::LSR, AddrMode::ZPX, 6}, // 0x56
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x57
    {Instruct::CLI, AddrMode::IMP, 2}, // 0x58
    {Instruct::EOR, AddrMode::ABY, 4}, // 0x59
    {Instruct::NOP, AddrMode::IMP, 2}, // 0x5A
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x5B
    {Instruct::NOP, AddrMode::ABX, 4}, // 0x5C
    {Instruct::EOR, AddrMode::ABX, 4}, // 0x5D
    {Instruct::LSR, AddrMode::AXP, 7}, // 0x5E
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x5F
    {Instruct::RTS, AddrMode::IMP, 6}, // 0x60
    {Instruct::ADC, AddrMode::IZX, 6}, // 0x61
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x62
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x63
    {Instruct::NOP, AddrMode::ZPG, 3}, // 0x64
    {Instruct::ADC, AddrMode::ZPG, 3}, // 0x65
    {Instruct::ROR, AddrMode::ZPG, 5}, // 0x66
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x67
    {Instruct::PLA, AddrMode::IMP, 4}, // 0x68
    {Instruct::ADI, AddrMode::IMM, 2}, // 0x69
    {Instruct::RRA, AddrMode::IMP, 2}, // 0x6A
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x6B
    {Instruct::JMP, AddrMode::IND, 5}, // 0x6C
    {Instruct::ADC, AddrMode::ABS, 4}, // 0x6D
    {Instruct::ROR, AddrMode::ABS, 6}, // 0x6E
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x6F
    {Instruct::BVS, AddrMode::REL, 2}, // 0x70
    {Instruct::ADC, AddrMode::IZY, 5}, // 0x71
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x72
    {Instruct::XXX, AddrMode::IMP, 8}, // 0x73
    {Instruct::NOP, AddrMode::ZPX, 4}, // 0x74
    {Instruct::ADC, AddrMode::ZPX, 4}, // 0x75
    {Instruct::ROR, AddrMode::ZPX, 6}, // 0x76
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x77
    {Instruct::SEI, AddrMode::IMP, 2}, // 0x78
    {Instruct::ADC, AddrMode::ABY, 4}, // 0x79
    {Instruct::NOP, AddrMode::IMP, 2}, // 0x7A
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x7B
    {Instruct::NOP, AddrMode::ABX, 4}, // 0x7C
    {Instruct::ADC, AddrMode::ABX, 4}, // 0x7D
    {Instruct::ROR, AddrMode::AXP, 7}, // 0x7E
    {Instruct::XXX, AddrMode::IMP, 7}, // 0x7F
    {Instruct::NOP, AddrMode::IMM, 2}, // 0x80
    {Instruct::STA, AddrMode::IZX, 6}, // 0x81
    {Instruct::NOP, AddrMode::IMP, 2}, // 0x82
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x83
    {Instruct::STY, AddrMode::ZPG, 3}, // 0x84
    {Instruct::STA, AddrMode::ZPG, 3}, // 0x85
    {Instruct::STX, AddrMode::ZPG, 3}, // 0x86
    {Instruct::XXX, AddrMode::IMP, 3}, // 0x87
    {Instruct::DEY, AddrMode::IMP, 2}, // 0x88
    {Instruct::NOP, AddrMode::IMP, 2}, // 0x89
    {Instruct::TXA, AddrMode::IMP, 2}, // 0x8A
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x8B
    {Instruct::STY, AddrMode::ABS, 4}, // 0x8C
    {Instruct::STA, AddrMode::ABS, 4}, // 0x8D
    {Instruct::STX, AddrMode::ABS, 4}, // 0x8E
    {Instruct::XXX, AddrMode::IMP, 4}, // 0x8F
    {Instruct::BCC, AddrMode::REL, 2}, // 0x90
    {Instruct::STA, AddrMode::IYP, 6}, // 0x91
    {Instruct::XXX, AddrMode::IMP, 2}, // 0x92
    {Instruct::XXX, AddrMode::IMP, 6}, // 0x93
    {Instruct::STY, AddrMode::ZPX, 4}, // 0x94
    {Instruct::STA, AddrMode::ZPX, 4}, // 0x95
    {Instruct::STX, AddrMode::ZPY, 4}, // 0x96
    {Instruct::XXX, AddrMode::IMP, 4}, // 0x97
    {Instruct::TYA, AddrMode::IMP, 2}, // 0x98
    {Instruct::STA, AddrMode::AYP, 5}, // 0x99
    {Instruct::TXS, AddrMode::IMP, 2}, // 0x9A
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x9B
    {Instruct::NOP, AddrMode::IMP, 5}, // 0x9C
    {Instruct::STA, AddrMode::AXP, 5}, // 0x9D
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x9E
    {Instruct::XXX, AddrMode::IMP, 5}, // 0x9F
    {Instruct::LYI, AddrMode::IMM, 2}, // 0xA0
    {Instruct::LDA, AddrMode::IZX, 6}, // 0xA1
    {Instruct::LXI, AddrMode::IMM, 2}, // 0xA2
    {Instruct::XXX, AddrMode::IMP, 6}, // 0xA3
    {Instruct::LDY, AddrMode::ZPG, 3}, // 0xA4
    {Instruct::LDA, AddrMode::ZPG, 3}, // 0xA5
    {Instruct::LDX, AddrMode::ZPG, 3}, // 0xA6
    {Instruct::XXX, AddrMode::IMP, 3}, // 0xA7
    {Instruct::TAY, AddrMode::IMP, 2}, // 0xA8
    {Instruct::LAI, AddrMode::IMM, 2}, // 0xA9
    {Instruct::TAX, AddrMode::IMP, 2}, // 0xAA
    {Instruct::XXX, AddrMode::IMP, 2}, // 0xAB
    {Instruct::LDY, AddrMode::ABS, 4}, // 0xAC
    {Instruct::LDA, AddrMode::ABS, 4}, // 0xAD
    {Instruct::LDX, AddrMode::ABS, 4}, // 0xAE
    {Instruct::XXX, AddrMode::IMP, 4}, // 0xAF
    {Instruct::BCS, AddrMode::REL, 2}, // 0xB0
    {Instruct::LDA, AddrMode::IZY, 5}, // 0xB1
    {Instruct::XXX, AddrMode::IMP, 2}, // 0xB2
    {Instruct::XXX, AddrMode::IMP, 5}, // 0xB3
    {Instruct::LDY, AddrMode::ZPX, 4}, // 0xB4
    {Instruct::LDA, AddrMode::ZPX, 4}, // 0xB5
    {Instruct::LDX, AddrMode::ZPY, 4}, // 0xB6
    {Instruct::XXX, AddrMode::IMP, 4}, // 0xB7
    {Instruct::CLV, AddrMode::IMP, 2}, // 0xB8
    {Instruct::LDA, AddrMode::ABY, 4}, // 0xB9
    {Instruct::TSX, AddrMode::IMP, 2}, // 0xBA
    {Instruct::XXX, AddrMode::IMP, 4}, // 0xBB
    {Instruct::LDY, AddrMode::ABX, 4}, // 0xBC
    {Instruct::LDA, AddrMode::ABX, 4}, // 0xBD
    {Instruct::LDX, AddrMode::ABY, 4}, // 0xBE
    {Instruct::XXX, AddrMode::IMP, 4}, // 0xBF
    {Instruct::CYI, AddrMode::IMM, 2}, // 0xC0
    {Instruct::CMP, AddrMode::IZX, 6}, // 0xC1
    {Instruct::NOP, AddrMode::IMP, 2}, // 0xC2
    {Instruct::XXX, AddrMode::IMP, 8}, // 0xC3
    {Instruct::CPY, AddrMode::ZPG, 3}, // 0xC4
    {Instruct::CMP, AddrMode::ZPG, 3}, // 0xC5
    {Instruct::DEC, AddrMode::ZPG, 5}, // 0xC6
    {Instruct::XXX, AddrMode::IMP, 5}, // 0xC7
    {Instruct::INY, AddrMode::IMP, 2}, // 0xC8
    {Instruct::CMI, AddrMode::IMM, 2}, // 0xC9
    {Instruct::DEX, AddrMode::IMP, 2}, // 0xCA
    {Instruct::XXX, AddrMode::IMP, 2}, // 0xCB
    {Instruct::CPY, AddrMode::ABS, 4}, // 0xCC
    {Instruct::CMP, AddrMode::ABS, 4}, // 0xCD
    {Instruct::DEC, AddrMode::ABS, 6}, // 0xCE
    {Instruct::XXX, AddrMode::IMP, 6}, // 0xCF
    {Instruct::BNE, AddrMode::REL, 2}, // 0xD0
    {Instruct::CMP, AddrMode::IZY, 5}, // 0xD1
    {Instruct::XXX, AddrMode::IMP, 2}, // 0xD2
    {Instruct::XXX, AddrMode::IMP, 8}, // 0xD3
    {Instruct::NOP, AddrMode::ZPX, 4}, // 0xD4
    {Instruct::CMP, AddrMode::ZPX, 4}, // 0xD5
    {Instruct::DEC, AddrMode::ZPX, 6}, // 0xD6
    {Instruct::XXX, AddrMode::IMP, 6}, // 0xD7
    {Instruct::CLD, AddrMode::IMP, 2}, // 0xD8
    {Instruct::CMP, AddrMode::ABY, 4}, // 0xD9
    {Instruct::NOP, AddrMode::IMP, 2}, // 0xDA
    {Instruct::XXX, AddrMode::IMP, 7}, // 0xDB
    {Instruct::NOP, AddrMode::ABX, 4}, // 0xDC
    {Instruct::CMP, AddrMode::ABX, 4}, // 0xDD
    {Instruct::DEC, AddrMode::AXP, 7}, // 0xDE
    {Instruct::XXX, AddrMode::IMP, 7}, // 0xDF
    {Instruct::CXI, AddrMode::IMM, 2}, // 0xE0
    {Instruct::SBC, AddrMode::IZX, 6}, // 0xE1
    {Instruct::NOP, AddrMode::IMP, 2}, // 0xE2
    {Instruct::XXX, AddrMode::IMP, 8}, // 0xE3
    {Instruct::CPX, AddrMode::ZPG, 3}, // 0xE4
    {Instruct::SBC, AddrMode::ZPG, 3}, // 0xE5
    {Instruct::INC, AddrMode::ZPG, 5}, // 0xE6
    {Instruct::XXX, AddrMode::IMP, 5}, // 0xE7
    {Instruct::INX, AddrMode::IMP, 2}, // 0xE8
    {Instruct::SBI, AddrMode::IMM, 2}, // 0xE9
    {Instruct::NOP, AddrMode::IMP, 2}, // 0xEA
    {Instruct::SBC, AddrMode::IMP, 2}, // 0xEB
    {Instruct::CPX, AddrMode::ABS, 4}, // 0xEC
    {Instruct::SBC, AddrMode::ABS, 4}, // 0xED
    {Instruct::INC, AddrMode::ABS, 6}, // 0xEE
    {Instruct::XXX, AddrMode::IMP, 6}, // 0xEF
    {Instruct::BEQ, AddrMode::REL, 2}, // 0xF0
    {Instruct::SBC, AddrMode::IZY, 5}, // 0xF1
    {Instruct::XXX, AddrMode::IMP, 2}, // 0xF2
    {Instruct::XXX, AddrMode::IMP, 8}, // 0xF3
    {Instruct::NOP, AddrMode::ZPX, 4}, // 0xF4
    {Instruct::SBC, AddrMode::ZPX, 4}, // 0xF5
    {Instruct::INC, AddrMode::ZPX, 6}, // 0xF6
    {Instruct::XXX, AddrMode::IMP, 6}, // 0xF7
    {Instruct::SED, AddrMode::IMP, 2}, // 0xF8
    {Instruct::SBC, AddrMode::ABY, 4}, // 0xF9
    {Instruct::NOP, AddrMode::IMP, 2}, // 0xFA
    {Instruct::XXX, AddrMode::IMP, 7}, // 0xFB
    {Instruct::NOP, AddrMode::ABX, 4}, // 0xFC
    {Instruct::SBC, AddrMode::ABX, 4}, // 0xFD
    {Instruct::INC, AddrMode::AXP, 7}, // 0xFE
    {Instruct::XXX, AddrMode::IMP, 7}, // 0xFF
};

// ----------------------------------------------------------------------------
// CPU class
// ----------------------------------------------------------------------------

// Constructor
CPU::CPU() {
    // keep only interrupt disable flag set
    RF.reg = 0b00000100;

    // Initialize registers
    PC = 0x0000;
    RA = RX = RY = 0x00;
    SP = SP_INIT;

    // Initialize cycles
    cycles = 0;
    cyc_count = 0;

    // Initialize temporary registers
    TDAT = 0x00;
    TABS = TREL = 0x0000;

    // Initialize memory to nullptr
    disk = nullptr;
}

// Destructor
CPU::~CPU() {}

// TODO: shared_ptr
void CPU::Mount(const Disk &disk) { this->disk = (Disk *)&disk; }

void CPU::Read() {
    // update total cycles, used when initializing in combination with RunInstr
    // TODO: remove this
    cyc_count += cycles;
    // update with new instruction
    addr = PC;
    Byte opcode = disk->ReadMBus(PC++);
    Operation op = map_op[opcode];
    mode = op.addrmode;
    instr = op.instruct;
    cycles = op.cycles;
    n_param = map_addrs[(uint8_t)op.addrmode];
    switch (n_param) {
    case 0:
        break;
    case 1:
        lhs = disk->ReadMBus(PC++);
        break;
    case 2:
        lhs = disk->ReadMBus(PC++);
        rhs = disk->ReadMBus(PC++);
        break;
    default:
        break;
    }
}

// Run one cycle
void CPU::RunCycle() {
    if (cycles == 0) {
        // fetch
        Read();
        // set unused flag
        RF.U = 1;
        // execute
        (this->*map_func_addrmode[(uint8_t)mode])();
        (this->*map_func_instruct[(uint8_t)instr])();
        // set unused flag
        RF.U = 1;
    }
    cycles--;
    cyc_count++;
}

// Run a complete instruction and return the number of cycles it takes
void CPU::RunInstr() {
    RF.U = 1;
    (this->*map_func_addrmode[(uint8_t)mode])();
    (this->*map_func_instruct[(uint8_t)instr])();
    RF.U = 1;
}

void CPU::Print() {
    std::string ins;

    ins = Misc::hex(addr, 4) + " <" + map_str_addrmode[(uint8_t)mode] + "> " +
          map_str_instruct[(uint8_t)instr];

    switch (mode) {
    case AddrMode::UNK:
    case AddrMode::IMP:
        ins += "      ";
        break;
    case AddrMode::IMM:
    case AddrMode::ZPG:
    case AddrMode::ZPX:
    case AddrMode::ZPY:
    case AddrMode::REL:
    case AddrMode::IZX:
    case AddrMode::IZY:
    case AddrMode::IYP:
        ins += " $" + Misc::hex(lhs, 2) + "  ";
        break;
    case AddrMode::ABS:
    case AddrMode::ABX:
    case AddrMode::AXP:
    case AddrMode::ABY:
    case AddrMode::AYP:
    case AddrMode::IND:
        ins += " $" + Misc::hex(rhs << 8 | lhs, 4);
        break;
    default:
        break;
    }

    ins += " A:" + Misc::hex(RA, 2) + " X:" + Misc::hex(RX, 2) +
           " Y:" + Misc::hex(RY, 2) + " P:" + Misc::hex(RF.reg, 2) +
           " SP:" + Misc::hex(SP, 2) + " TABS:" + Misc::hex(TABS, 4) +
           " CYC:" + std::to_string(cyc_count);

    std::cout << ins << std::endl;
}
