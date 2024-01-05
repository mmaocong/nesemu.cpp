#include <iostream>
#include <string>

#include "cpu.hpp"
#include "misc.hpp"

// address mode
enum class AddrMode : uint8_t {
    UNK = 0, // Unknown
    IMP,     // Implied, assume RA as operand, no data fetched
    IMM,     // Immediate
    ZPG,     // Zero Page
    ZPX,     // Zero Page, X
    ZPY,     // Zero Page, Y
    REL,     // Relative
    ABS,     // Absolute
    ABX,     // Absolute, X
    ABY,     // Absolute, Y
    IND,     // Indirect
    IZX,     // Indexed Indirect, X
    IZY,     // Indirect Indexed, Y
};

enum class Instruct : uint8_t {
    UNK = 0, // Unknown
    BRK,
    ORA,
    XXX,
    NOP,
    ASL,
    PHP,
    CLC,
    JSR,
    BIT,
    AND,
    ROL,
    PLP,
    BMI,
    SEC,
    RTI,
    EOR,
    LSR,
    PHA,
    JMP,
    BVC,
    CLI,
    RTS,
    ADC,
    ROR,
    PLA,
    BVS,
    SEI,
    STA,
    STY,
    STX,
    DEY,
    TXA,
    BCC,
    TYA,
    TXS,
    LDY,
    LDA,
    LDX,
    TAY,
    TAX,
    BCS,
    CLV,
    TSX,
    CPY,
    CMP,
    DEC,
    INY,
    DEX,
    BNE,
    CLD,
    CPX,
    SBC,
    INC,
    INX,
    BEQ,
    SED,
    BPL,
};

struct Operation {
    void (CPU::*func_instruct)(void) = nullptr;
    void (CPU::*func_addrmode)(void) = nullptr;
    Instruct instruct = Instruct::UNK;
    AddrMode addrmode = AddrMode::UNK;
    uint8_t cycles = 0;
    char name[4] = "XXX";
};

static const std::vector<Operation> lookup = {
    {&CPU::BRK, &CPU::IMM, Instruct::BRK, AddrMode::IMM, 7, "BRK"},
    {&CPU::ORA, &CPU::IZX, Instruct::ORA, AddrMode::IZX, 6, "ORA"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPG, Instruct::NOP, AddrMode::ZPG, 3, "XXX"},
    {&CPU::ORA, &CPU::ZPG, Instruct::ORA, AddrMode::ZPG, 3, "ORA"},
    {&CPU::ASL, &CPU::ZPG, Instruct::ASL, AddrMode::ZPG, 5, "ASL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::PHP, &CPU::IMP, Instruct::PHP, AddrMode::IMP, 3, "PHP"},
    {&CPU::ORA, &CPU::IMM, Instruct::ORA, AddrMode::IMM, 2, "ORA"},
    {&CPU::ALA, &CPU::IMP, Instruct::ASL, AddrMode::IMP, 2, "ASL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::ABS, Instruct::NOP, AddrMode::ABS, 4, "XXX"},
    {&CPU::ORA, &CPU::ABS, Instruct::ORA, AddrMode::ABS, 4, "ORA"},
    {&CPU::ASL, &CPU::ABS, Instruct::ASL, AddrMode::ABS, 6, "ASL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::BPL, &CPU::REL, Instruct::BPL, AddrMode::REL, 2, "BPL"},
    {&CPU::ORA, &CPU::IZY, Instruct::ORA, AddrMode::IZY, 5, "ORA"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPX, Instruct::NOP, AddrMode::ZPX, 4, "XXX"},
    {&CPU::ORA, &CPU::ZPX, Instruct::ORA, AddrMode::ZPX, 4, "ORA"},
    {&CPU::ASL, &CPU::ZPX, Instruct::ASL, AddrMode::ZPX, 6, "ASL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::CLC, &CPU::IMP, Instruct::CLC, AddrMode::IMP, 2, "CLC"},
    {&CPU::ORA, &CPU::ABY, Instruct::ORA, AddrMode::ABY, 4, "ORA"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::ABX, Instruct::NOP, AddrMode::ABX, 4, "XXX"},
    {&CPU::ORA, &CPU::ABX, Instruct::ORA, AddrMode::ABX, 4, "ORA"},
    {&CPU::ASL, &CPU::AXP, Instruct::ASL, AddrMode::ABX, 7, "ASL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::JSR, &CPU::ABS, Instruct::JSR, AddrMode::ABS, 6, "JSR"},
    {&CPU::AND, &CPU::IZX, Instruct::AND, AddrMode::IZX, 6, "AND"},
    {&CPU::XXX, &CPU::ZPX, Instruct::XXX, AddrMode::ZPX, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::BIT, &CPU::ZPG, Instruct::BIT, AddrMode::ZPG, 3, "BIT"},
    {&CPU::AND, &CPU::ZPG, Instruct::AND, AddrMode::ZPG, 3, "AND"},
    {&CPU::ROL, &CPU::ZPG, Instruct::ROL, AddrMode::ZPG, 5, "ROL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::PLP, &CPU::IMP, Instruct::PLP, AddrMode::IMP, 4, "PLP"},
    {&CPU::AND, &CPU::IMM, Instruct::AND, AddrMode::IMM, 2, "AND"},
    {&CPU::RLA, &CPU::IMP, Instruct::ROL, AddrMode::IMP, 2, "ROL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::BIT, &CPU::ABS, Instruct::BIT, AddrMode::ABS, 4, "BIT"},
    {&CPU::AND, &CPU::ABS, Instruct::AND, AddrMode::ABS, 4, "AND"},
    {&CPU::ROL, &CPU::ABS, Instruct::ROL, AddrMode::ABS, 6, "ROL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::BMI, &CPU::REL, Instruct::BMI, AddrMode::REL, 2, "BMI"},
    {&CPU::AND, &CPU::IZY, Instruct::AND, AddrMode::IZY, 5, "AND"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPX, Instruct::NOP, AddrMode::ZPX, 4, "XXX"},
    {&CPU::AND, &CPU::ZPX, Instruct::AND, AddrMode::ZPX, 4, "AND"},
    {&CPU::ROL, &CPU::ZPX, Instruct::ROL, AddrMode::ZPX, 6, "ROL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::SEC, &CPU::IMP, Instruct::SEC, AddrMode::IMP, 2, "SEC"},
    {&CPU::AND, &CPU::ABY, Instruct::AND, AddrMode::ABY, 4, "AND"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::ABX, Instruct::NOP, AddrMode::ABX, 4, "XXX"},
    {&CPU::AND, &CPU::ABX, Instruct::AND, AddrMode::ABX, 4, "AND"},
    {&CPU::ROL, &CPU::AXP, Instruct::ROL, AddrMode::ABX, 7, "ROL"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::RTI, &CPU::IMP, Instruct::RTI, AddrMode::IMP, 6, "RTI"},
    {&CPU::EOR, &CPU::IZX, Instruct::EOR, AddrMode::IZX, 6, "EOR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPG, Instruct::NOP, AddrMode::ZPG, 3, "XXX"},
    {&CPU::EOR, &CPU::ZPG, Instruct::EOR, AddrMode::ZPG, 3, "EOR"},
    {&CPU::LSR, &CPU::ZPG, Instruct::LSR, AddrMode::ZPG, 5, "LSR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::PHA, &CPU::IMP, Instruct::PHA, AddrMode::IMP, 3, "PHA"},
    {&CPU::EOR, &CPU::IMM, Instruct::EOR, AddrMode::IMM, 2, "EOR"},
    {&CPU::LRA, &CPU::IMP, Instruct::LSR, AddrMode::IMP, 2, "LSR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::JMP, &CPU::ABS, Instruct::JMP, AddrMode::ABS, 3, "JMP"},
    {&CPU::EOR, &CPU::ABS, Instruct::EOR, AddrMode::ABS, 4, "EOR"},
    {&CPU::LSR, &CPU::ABS, Instruct::LSR, AddrMode::ABS, 6, "LSR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::BVC, &CPU::REL, Instruct::BVC, AddrMode::REL, 2, "BVC"},
    {&CPU::EOR, &CPU::IZY, Instruct::EOR, AddrMode::IZY, 5, "EOR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPX, Instruct::NOP, AddrMode::ZPX, 4, "XXX"},
    {&CPU::EOR, &CPU::ZPX, Instruct::EOR, AddrMode::ZPX, 4, "EOR"},
    {&CPU::LSR, &CPU::ZPX, Instruct::LSR, AddrMode::ZPX, 6, "LSR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::CLI, &CPU::IMP, Instruct::CLI, AddrMode::IMP, 2, "CLI"},
    {&CPU::EOR, &CPU::ABY, Instruct::EOR, AddrMode::ABY, 4, "EOR"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::ABX, Instruct::NOP, AddrMode::ABX, 4, "XXX"},
    {&CPU::EOR, &CPU::ABX, Instruct::EOR, AddrMode::ABX, 4, "EOR"},
    {&CPU::LSR, &CPU::AXP, Instruct::LSR, AddrMode::ABX, 7, "LSR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::RTS, &CPU::IMP, Instruct::RTS, AddrMode::IMP, 6, "RTS"},
    {&CPU::ADC, &CPU::IZX, Instruct::ADC, AddrMode::IZX, 6, "ADC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPG, Instruct::NOP, AddrMode::ZPG, 3, "XXX"},
    {&CPU::ADC, &CPU::ZPG, Instruct::ADC, AddrMode::ZPG, 3, "ADC"},
    {&CPU::ROR, &CPU::ZPG, Instruct::ROR, AddrMode::ZPG, 5, "ROR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::PLA, &CPU::IMP, Instruct::PLA, AddrMode::IMP, 4, "PLA"},
    {&CPU::ADC, &CPU::IMM, Instruct::ADC, AddrMode::IMM, 2, "ADC"},
    {&CPU::RRA, &CPU::IMP, Instruct::ROR, AddrMode::IMP, 2, "ROR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::JMP, &CPU::IND, Instruct::JMP, AddrMode::IND, 5, "JMP"},
    {&CPU::ADC, &CPU::ABS, Instruct::ADC, AddrMode::ABS, 4, "ADC"},
    {&CPU::ROR, &CPU::ABS, Instruct::ROR, AddrMode::ABS, 6, "ROR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::BVS, &CPU::REL, Instruct::BVS, AddrMode::REL, 2, "BVS"},
    {&CPU::ADC, &CPU::IZY, Instruct::ADC, AddrMode::IZY, 5, "ADC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPX, Instruct::NOP, AddrMode::ZPX, 4, "XXX"},
    {&CPU::ADC, &CPU::ZPX, Instruct::ADC, AddrMode::ZPX, 4, "ADC"},
    {&CPU::ROR, &CPU::ZPX, Instruct::ROR, AddrMode::ZPX, 6, "ROR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::SEI, &CPU::IMP, Instruct::SEI, AddrMode::IMP, 2, "SEI"},
    {&CPU::ADC, &CPU::ABY, Instruct::ADC, AddrMode::ABY, 4, "ADC"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::ABX, Instruct::NOP, AddrMode::ABX, 4, "XXX"},
    {&CPU::ADC, &CPU::ABX, Instruct::ADC, AddrMode::ABX, 4, "ADC"},
    {&CPU::ROR, &CPU::AXP, Instruct::ROR, AddrMode::ABX, 7, "ROR"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::IMM, Instruct::NOP, AddrMode::IMM, 2, "XXX"},
    {&CPU::STA, &CPU::IZX, Instruct::STA, AddrMode::IZX, 6, "STA"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::STY, &CPU::ZPG, Instruct::STY, AddrMode::ZPG, 3, "STY"},
    {&CPU::STA, &CPU::ZPG, Instruct::STA, AddrMode::ZPG, 3, "STA"},
    {&CPU::STX, &CPU::ZPG, Instruct::STX, AddrMode::ZPG, 3, "STX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 3, "XXX"},
    {&CPU::DEY, &CPU::IMP, Instruct::DEY, AddrMode::IMP, 2, "DEY"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::TXA, &CPU::IMP, Instruct::TXA, AddrMode::IMP, 2, "TXA"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::STY, &CPU::ABS, Instruct::STY, AddrMode::ABS, 4, "STY"},
    {&CPU::STA, &CPU::ABS, Instruct::STA, AddrMode::ABS, 4, "STA"},
    {&CPU::STX, &CPU::ABS, Instruct::STX, AddrMode::ABS, 4, "STX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 4, "XXX"},
    {&CPU::BCC, &CPU::REL, Instruct::BCC, AddrMode::REL, 2, "BCC"},
    {&CPU::STA, &CPU::IYP, Instruct::STA, AddrMode::IZY, 6, "STA"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::STY, &CPU::ZPX, Instruct::STY, AddrMode::ZPX, 4, "STY"},
    {&CPU::STA, &CPU::ZPX, Instruct::STA, AddrMode::ZPX, 4, "STA"},
    {&CPU::STX, &CPU::ZPY, Instruct::STX, AddrMode::ZPY, 4, "STX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 4, "XXX"},
    {&CPU::TYA, &CPU::IMP, Instruct::TYA, AddrMode::IMP, 2, "TYA"},
    {&CPU::STA, &CPU::AYP, Instruct::STA, AddrMode::ABY, 5, "STA"},
    {&CPU::TXS, &CPU::IMP, Instruct::TXS, AddrMode::IMP, 2, "TXS"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 5, "XXX"},
    {&CPU::STA, &CPU::AXP, Instruct::STA, AddrMode::ABX, 5, "STA"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::LDY, &CPU::IMM, Instruct::LDY, AddrMode::IMM, 2, "LDY"},
    {&CPU::LDA, &CPU::IZX, Instruct::LDA, AddrMode::IZX, 6, "LDA"},
    {&CPU::LDX, &CPU::IMM, Instruct::LDX, AddrMode::IMM, 2, "LDX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::LDY, &CPU::ZPG, Instruct::LDY, AddrMode::ZPG, 3, "LDY"},
    {&CPU::LDA, &CPU::ZPG, Instruct::LDA, AddrMode::ZPG, 3, "LDA"},
    {&CPU::LDX, &CPU::ZPG, Instruct::LDX, AddrMode::ZPG, 3, "LDX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 3, "XXX"},
    {&CPU::TAY, &CPU::IMP, Instruct::TAY, AddrMode::IMP, 2, "TAY"},
    {&CPU::LDA, &CPU::IMM, Instruct::LDA, AddrMode::IMM, 2, "LDA"},
    {&CPU::TAX, &CPU::IMP, Instruct::TAX, AddrMode::IMP, 2, "TAX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::LDY, &CPU::ABS, Instruct::LDY, AddrMode::ABS, 4, "LDY"},
    {&CPU::LDA, &CPU::ABS, Instruct::LDA, AddrMode::ABS, 4, "LDA"},
    {&CPU::LDX, &CPU::ABS, Instruct::LDX, AddrMode::ABS, 4, "LDX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 4, "XXX"},
    {&CPU::BCS, &CPU::REL, Instruct::BCS, AddrMode::REL, 2, "BCS"},
    {&CPU::LDA, &CPU::IZY, Instruct::LDA, AddrMode::IZY, 5, "LDA"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::LDY, &CPU::ZPX, Instruct::LDY, AddrMode::ZPX, 4, "LDY"},
    {&CPU::LDA, &CPU::ZPX, Instruct::LDA, AddrMode::ZPX, 4, "LDA"},
    {&CPU::LDX, &CPU::ZPY, Instruct::LDX, AddrMode::ZPY, 4, "LDX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 4, "XXX"},
    {&CPU::CLV, &CPU::IMP, Instruct::CLV, AddrMode::IMP, 2, "CLV"},
    {&CPU::LDA, &CPU::ABY, Instruct::LDA, AddrMode::ABY, 4, "LDA"},
    {&CPU::TSX, &CPU::IMP, Instruct::TSX, AddrMode::IMP, 2, "TSX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 4, "XXX"},
    {&CPU::LDY, &CPU::ABX, Instruct::LDY, AddrMode::ABX, 4, "LDY"},
    {&CPU::LDA, &CPU::ABX, Instruct::LDA, AddrMode::ABX, 4, "LDA"},
    {&CPU::LDX, &CPU::ABY, Instruct::LDX, AddrMode::ABY, 4, "LDX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 4, "XXX"},
    {&CPU::CPY, &CPU::IMM, Instruct::CPY, AddrMode::IMM, 2, "CPY"},
    {&CPU::CMP, &CPU::IZX, Instruct::CMP, AddrMode::IZX, 6, "CMP"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::CPY, &CPU::ZPG, Instruct::CPY, AddrMode::ZPG, 3, "CPY"},
    {&CPU::CMP, &CPU::ZPG, Instruct::CMP, AddrMode::ZPG, 3, "CMP"},
    {&CPU::DEC, &CPU::ZPG, Instruct::DEC, AddrMode::ZPG, 5, "DEC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::INY, &CPU::IMP, Instruct::INY, AddrMode::IMP, 2, "INY"},
    {&CPU::CMP, &CPU::IMM, Instruct::CMP, AddrMode::IMM, 2, "CMP"},
    {&CPU::DEX, &CPU::IMP, Instruct::DEX, AddrMode::IMP, 2, "DEX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::CPY, &CPU::ABS, Instruct::CPY, AddrMode::ABS, 4, "CPY"},
    {&CPU::CMP, &CPU::ABS, Instruct::CMP, AddrMode::ABS, 4, "CMP"},
    {&CPU::DEC, &CPU::ABS, Instruct::DEC, AddrMode::ABS, 6, "DEC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::BNE, &CPU::REL, Instruct::BNE, AddrMode::REL, 2, "BNE"},
    {&CPU::CMP, &CPU::IZY, Instruct::CMP, AddrMode::IZY, 5, "CMP"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPX, Instruct::NOP, AddrMode::ZPX, 4, "XXX"},
    {&CPU::CMP, &CPU::ZPX, Instruct::CMP, AddrMode::ZPX, 4, "CMP"},
    {&CPU::DEC, &CPU::ZPX, Instruct::DEC, AddrMode::ZPX, 6, "DEC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::CLD, &CPU::IMP, Instruct::CLD, AddrMode::IMP, 2, "CLD"},
    {&CPU::CMP, &CPU::ABY, Instruct::CMP, AddrMode::ABY, 4, "CMP"},
    {&CPU::NOP, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "NOP"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::ABX, Instruct::NOP, AddrMode::ABX, 4, "XXX"},
    {&CPU::CMP, &CPU::ABX, Instruct::CMP, AddrMode::ABX, 4, "CMP"},
    {&CPU::DEC, &CPU::AXP, Instruct::DEC, AddrMode::ABX, 7, "DEC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::CPX, &CPU::IMM, Instruct::CPX, AddrMode::IMM, 2, "CPX"},
    {&CPU::SBC, &CPU::IZX, Instruct::SBC, AddrMode::IZX, 6, "SBC"},
    {&CPU::XXX, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::CPX, &CPU::ZPG, Instruct::CPX, AddrMode::ZPG, 3, "CPX"},
    {&CPU::SBC, &CPU::ZPG, Instruct::SBC, AddrMode::ZPG, 3, "SBC"},
    {&CPU::INC, &CPU::ZPG, Instruct::INC, AddrMode::ZPG, 5, "INC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 5, "XXX"},
    {&CPU::INX, &CPU::IMP, Instruct::INX, AddrMode::IMP, 2, "INX"},
    {&CPU::SBC, &CPU::IMM, Instruct::SBC, AddrMode::IMM, 2, "SBC"},
    {&CPU::NOP, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "NOP"},
    {&CPU::XXX, &CPU::IMP, Instruct::SBC, AddrMode::IMP, 2, "XXX"},
    {&CPU::CPX, &CPU::ABS, Instruct::CPX, AddrMode::ABS, 4, "CPX"},
    {&CPU::SBC, &CPU::ABS, Instruct::SBC, AddrMode::ABS, 4, "SBC"},
    {&CPU::INC, &CPU::ABS, Instruct::INC, AddrMode::ABS, 6, "INC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::BEQ, &CPU::REL, Instruct::BEQ, AddrMode::REL, 2, "BEQ"},
    {&CPU::SBC, &CPU::IZY, Instruct::SBC, AddrMode::IZY, 5, "SBC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 2, "XXX"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 8, "XXX"},
    {&CPU::XXX, &CPU::ZPX, Instruct::NOP, AddrMode::ZPX, 4, "XXX"},
    {&CPU::SBC, &CPU::ZPX, Instruct::SBC, AddrMode::ZPX, 4, "SBC"},
    {&CPU::INC, &CPU::ZPX, Instruct::INC, AddrMode::ZPX, 6, "INC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 6, "XXX"},
    {&CPU::SED, &CPU::IMP, Instruct::SED, AddrMode::IMP, 2, "SED"},
    {&CPU::SBC, &CPU::ABY, Instruct::SBC, AddrMode::ABY, 4, "SBC"},
    {&CPU::NOP, &CPU::IMP, Instruct::NOP, AddrMode::IMP, 2, "NOP"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
    {&CPU::XXX, &CPU::ABX, Instruct::NOP, AddrMode::ABX, 4, "XXX"},
    {&CPU::SBC, &CPU::ABX, Instruct::SBC, AddrMode::ABX, 4, "SBC"},
    {&CPU::INC, &CPU::AXP, Instruct::INC, AddrMode::ABX, 7, "INC"},
    {&CPU::XXX, &CPU::IMP, Instruct::XXX, AddrMode::IMP, 7, "XXX"},
};

// ----------------------------------------------------------------------------
// CPU class
// ----------------------------------------------------------------------------

// Constructor
CPU::CPU() {
    // Initialize registers
    RA = RX = RY = 0x00;
    SP = SP_INIT;
    PC = 0x0000;
    // keep only interrupt disable flag set
    RF.reg = 0b00000100;

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

// show one instruction
void CPU::BinToAsm(const Mem &buffer, std::vector<std::string> &asmcode) {
    uint16_t limit = MIN(buffer.size(), UINT16_MAX);
    uint16_t addr = 0;
    std::string ins;
    while (addr < limit) {
        ins = "$" + Misc::hex(addr, 4) + ": ";
        Byte ind_op = buffer[addr++];
        Operation op = lookup[ind_op];
        ins = ins + op.name + " ";
        Byte val, lo, hi;
        switch (op.addrmode) {
        case AddrMode::IMP:
            // do NOT proceed the PC here
            ins += "<IMP>";
            break;
        case AddrMode::IMM:
            val = buffer[addr++];
            ins += Misc::hex(val, 2) + " <IMM>";
            break;
        case AddrMode::ZPG:
            lo = buffer[addr++];
            hi = 0x00;
            ins += Misc::hex(lo, 2) + " <ZPG>";
            break;
        case AddrMode::ZPX:
            lo = buffer[addr++];
            hi = 0x00;
            ins += Misc::hex(lo, 2) + ",X <ZPX>";
            break;
        case AddrMode::ZPY:
            lo = buffer[addr++];
            hi = 0x00;
            ins += Misc::hex(lo, 2) + ",Y <ZPY>";
            break;
        case AddrMode::REL:
            val = buffer[addr++];
            ins +=
                Misc::hex(val, 2) + ", " + Misc::hex(addr + val, 4) + " <REL>";
            break;
        case AddrMode::ABS:
            lo = buffer[addr++];
            hi = buffer[addr++];
            ins += "$" + Misc::hex(hi << 8 | lo, 4) + " <ABS>";
            break;
        case AddrMode::ABX:
            lo = buffer[addr++];
            hi = buffer[addr++];
            ins += "$" + Misc::hex(hi << 8 | lo, 4) + ", X <ABX>";
            break;
        case AddrMode::ABY:
            lo = buffer[addr++];
            hi = buffer[addr++];
            ins += "$" + Misc::hex(hi << 8 | lo, 4) + ", Y <ABY>";
            break;
        case AddrMode::IND:
            lo = buffer[addr++];
            hi = buffer[addr++];
            ins += Misc::hex(hi << 8 | lo, 4) + " <IND>";
            break;
        case AddrMode::IZX:
            lo = buffer[addr++];
            hi = 0x00;
            ins += "(" + Misc::hex(lo, 2) + ", X) <IZX>";
            break;
        case AddrMode::IZY:
            lo = buffer[addr++];
            hi = 0x00;
            ins += "(" + Misc::hex(lo, 2) + ", Y) <IZY>";
            break;
        default:
            addr++;
            ins += " <UNK>";
            break;
        }
        asmcode.push_back(ins);
    }
}

// Run one cycle
void CPU::RunCycle() {
    if (cycles == 0) {
        // fetch
        Byte ind_op = disk->ReadMBus(PC++);
        // set unused flag
        RF.U = 1;
        Operation op = lookup[ind_op];
        // update cycles
        cycles = op.cycles;
        // execute
        (this->*op.func_addrmode)();
        (this->*op.func_instruct)();
        // set unused flag
        RF.U = 1;
    }
    cycles--;
}

// Run a complete instruction and return the number of cycles it takes
uint8_t CPU::RunInstr() {
    Byte opcode = disk->ReadMBus(PC++);
    Operation op = lookup[opcode];
    // execute
    RF.U = 1;
    (this->*op.func_addrmode)();
    (this->*op.func_instruct)();
    RF.U = 1;
    // setting cycles
    uint8_t res = op.cycles + cycles;
    cycles = 0;
    return res;
}

// Execute all instructions for debugging
void CPU::Exec(const uint16_t &addr, const size_t &n) {
    PC = addr;
    size_t cyc_count = 0;
    for (size_t i = 0; i < n; i++) {

        std::cout << "$" << std::hex << Misc::hex(PC, 4) << ": ";

        Byte ind_op = disk->ReadMBus(PC);
        Operation op = lookup[ind_op];

        cyc_count += cycles;
        std::cout << op.name << " <" << +(uint8_t)op.addrmode << "> "
                  << "; A: " << Misc::hex(RA, 2) << "; X: " << Misc::hex(RX, 2)
                  << "; Y: " << Misc::hex(RY, 2)
                  << "; P: " << Misc::hex(RF.reg, 2)
                  << "; SP: " << Misc::hex(SP, 2) << std::dec
                  << "; CYC: " << +cyc_count << std::endl;
        PC++;

        // update cycles
        cycles = op.cycles;
        // execute
        RF.U = 1;
        (this->*op.func_addrmode)();
        (this->*op.func_instruct)();
        RF.U = 1;
    }
}

void CPU::Print() {
    std::cout << "A:" << Misc::hex(RA, 2) << " X:" << Misc::hex(RX, 2)
              << " Y:" << Misc::hex(RY, 2) << " P:" << Misc::hex(RF.reg, 2)
              << " SP:" << Misc::hex(SP, 2) << std::endl;
}
