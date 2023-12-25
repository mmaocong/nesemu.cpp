#include <string>

#include "cpu.hpp"

/*
// lookup table for instructions
std::vector<Instruction> lookup = {
    {"BRK", Instruction::BRK, AddrMode::IMM, 7},
    {"ORA", Instruction::ORA, AddrMode::IZX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 3},
    {"ORA", Instruction::ORA, AddrMode::ZPG, 3},
    {"ASL", Instruction::ASL, AddrMode::ZPG, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"PHP", Instruction::PHP, AddrMode::IMP, 3},
    {"ORA", Instruction::ORA, AddrMode::IMM, 2},
    {"ASL", Instruction::ASL, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"ORA", Instruction::ORA, AddrMode::ABS, 4},
    {"ASL", Instruction::ASL, AddrMode::ABS, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"BPL", Instruction::BPL, AddrMode::REL, 2},
    {"ORA", Instruction::ORA, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"ORA", Instruction::ORA, AddrMode::ZPX, 4},
    {"ASL", Instruction::ASL, AddrMode::ZPX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"CLC", Instruction::CLC, AddrMode::IMP, 2},
    {"ORA", Instruction::ORA, AddrMode::ABY, 4},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"ORA", Instruction::ORA, AddrMode::ABX, 4},
    {"ASL", Instruction::ASL, AddrMode::ABX, 7},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"JSR", Instruction::JSR, AddrMode::ABS, 6},
    {"AND", Instruction::AND, AddrMode::IZX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"BIT", Instruction::BIT, AddrMode::ZPG, 3},
    {"AND", Instruction::AND, AddrMode::ZPG, 3},
    {"ROL", Instruction::ROL, AddrMode::ZPG, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"PLP", Instruction::PLP, AddrMode::IMP, 4},
    {"AND", Instruction::AND, AddrMode::IMM, 2},
    {"ROL", Instruction::ROL, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"BIT", Instruction::BIT, AddrMode::ABS, 4},
    {"AND", Instruction::AND, AddrMode::ABS, 4},
    {"ROL", Instruction::ROL, AddrMode::ABS, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"BMI", Instruction::BMI, AddrMode::REL, 2},
    {"AND", Instruction::AND, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"AND", Instruction::AND, AddrMode::ZPX, 4},
    {"ROL", Instruction::ROL, AddrMode::ZPX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"SEC", Instruction::SEC, AddrMode::IMP, 2},
    {"AND", Instruction::AND, AddrMode::ABY, 4},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"AND", Instruction::AND, AddrMode::ABX, 4},
    {"ROL", Instruction::ROL, AddrMode::ABX, 7},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"RTI", Instruction::RTI, AddrMode::IMP, 6},
    {"EOR", Instruction::EOR, AddrMode::IZX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 3},
    {"EOR", Instruction::EOR, AddrMode::ZPG, 3},
    {"LSR", Instruction::LSR, AddrMode::ZPG, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"PHA", Instruction::PHA, AddrMode::IMP, 3},
    {"EOR", Instruction::EOR, AddrMode::IMM, 2},
    {"LSR", Instruction::LSR, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"JMP", Instruction::JMP, AddrMode::ABS, 3},
    {"EOR", Instruction::EOR, AddrMode::ABS, 4},
    {"LSR", Instruction::LSR, AddrMode::ABS, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"BVC", Instruction::BVC, AddrMode::REL, 2},
    {"EOR", Instruction::EOR, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"EOR", Instruction::EOR, AddrMode::ZPX, 4},
    {"LSR", Instruction::LSR, AddrMode::ZPX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"CLI", Instruction::CLI, AddrMode::IMP, 2},
    {"EOR", Instruction::EOR, AddrMode::ABY, 4},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"EOR", Instruction::EOR, AddrMode::ABX, 4},
    {"LSR", Instruction::LSR, AddrMode::ABX, 7},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"RTS", Instruction::RTS, AddrMode::IMP, 6},
    {"ADC", Instruction::ADC, AddrMode::IZX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 3},
    {"ADC", Instruction::ADC, AddrMode::ZPG, 3},
    {"ROR", Instruction::ROR, AddrMode::ZPG, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"PLA", Instruction::PLA, AddrMode::IMP, 4},
    {"ADC", Instruction::ADC, AddrMode::IMM, 2},
    {"ROR", Instruction::ROR, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"JMP", Instruction::JMP, AddrMode::IND, 5},
    {"ADC", Instruction::ADC, AddrMode::ABS, 4},
    {"ROR", Instruction::ROR, AddrMode::ABS, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"BVS", Instruction::BVS, AddrMode::REL, 2},
    {"ADC", Instruction::ADC, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"ADC", Instruction::ADC, AddrMode::ZPX, 4},
    {"ROR", Instruction::ROR, AddrMode::ZPX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"SEI", Instruction::SEI, AddrMode::IMP, 2},
    {"ADC", Instruction::ADC, AddrMode::ABY, 4},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"ADC", Instruction::ADC, AddrMode::ABX, 4},
    {"ROR", Instruction::ROR, AddrMode::ABX, 7},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"STA", Instruction::STA, AddrMode::IZX, 6},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"STY", Instruction::STY, AddrMode::ZPG, 3},
    {"STA", Instruction::STA, AddrMode::ZPG, 3},
    {"STX", Instruction::STX, AddrMode::ZPG, 3},
    {"XXX", Instruction::XXX, AddrMode::IMP, 3},
    {"DEY", Instruction::DEY, AddrMode::IMP, 2},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"TXA", Instruction::TXA, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"STY", Instruction::STY, AddrMode::ABS, 4},
    {"STA", Instruction::STA, AddrMode::ABS, 4},
    {"STX", Instruction::STX, AddrMode::ABS, 4},
    {"XXX", Instruction::XXX, AddrMode::IMP, 4},
    {"BCC", Instruction::BCC, AddrMode::REL, 2},
    {"STA", Instruction::STA, AddrMode::IZY, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"STY", Instruction::STY, AddrMode::ZPX, 4},
    {"STA", Instruction::STA, AddrMode::ZPX, 4},
    {"STX", Instruction::STX, AddrMode::ZPY, 4},
    {"XXX", Instruction::XXX, AddrMode::IMP, 4},
    {"TYA", Instruction::TYA, AddrMode::IMP, 2},
    {"STA", Instruction::STA, AddrMode::ABY, 5},
    {"TXS", Instruction::TXS, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"XXX", Instruction::NOP, AddrMode::IMP, 5},
    {"STA", Instruction::STA, AddrMode::ABX, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"LDY", Instruction::LDY, AddrMode::IMM, 2},
    {"LDA", Instruction::LDA, AddrMode::IZX, 6},
    {"LDX", Instruction::LDX, AddrMode::IMM, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"LDY", Instruction::LDY, AddrMode::ZPG, 3},
    {"LDA", Instruction::LDA, AddrMode::ZPG, 3},
    {"LDX", Instruction::LDX, AddrMode::ZPG, 3},
    {"XXX", Instruction::XXX, AddrMode::IMP, 3},
    {"TAY", Instruction::TAY, AddrMode::IMP, 2},
    {"LDA", Instruction::LDA, AddrMode::IMM, 2},
    {"TAX", Instruction::TAX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"LDY", Instruction::LDY, AddrMode::ABS, 4},
    {"LDA", Instruction::LDA, AddrMode::ABS, 4},
    {"LDX", Instruction::LDX, AddrMode::ABS, 4},
    {"XXX", Instruction::XXX, AddrMode::IMP, 4},
    {"BCS", Instruction::BCS, AddrMode::REL, 2},
    {"LDA", Instruction::LDA, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"LDY", Instruction::LDY, AddrMode::ZPX, 4},
    {"LDA", Instruction::LDA, AddrMode::ZPX, 4},
    {"LDX", Instruction::LDX, AddrMode::ZPY, 4},
    {"XXX", Instruction::XXX, AddrMode::IMP, 4},
    {"CLV", Instruction::CLV, AddrMode::IMP, 2},
    {"LDA", Instruction::LDA, AddrMode::ABY, 4},
    {"TSX", Instruction::TSX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 4},
    {"LDY", Instruction::LDY, AddrMode::ABX, 4},
    {"LDA", Instruction::LDA, AddrMode::ABX, 4},
    {"LDX", Instruction::LDX, AddrMode::ABY, 4},
    {"XXX", Instruction::XXX, AddrMode::IMP, 4},
    {"CPY", Instruction::CPY, AddrMode::IMM, 2},
    {"CMP", Instruction::CMP, AddrMode::IZX, 6},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"CPY", Instruction::CPY, AddrMode::ZPG, 3},
    {"CMP", Instruction::CMP, AddrMode::ZPG, 3},
    {"DEC", Instruction::DEC, AddrMode::ZPG, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"INY", Instruction::INY, AddrMode::IMP, 2},
    {"CMP", Instruction::CMP, AddrMode::IMM, 2},
    {"DEX", Instruction::DEX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"CPY", Instruction::CPY, AddrMode::ABS, 4},
    {"CMP", Instruction::CMP, AddrMode::ABS, 4},
    {"DEC", Instruction::DEC, AddrMode::ABS, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"BNE", Instruction::BNE, AddrMode::REL, 2},
    {"CMP", Instruction::CMP, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"CMP", Instruction::CMP, AddrMode::ZPX, 4},
    {"DEC", Instruction::DEC, AddrMode::ZPX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"CLD", Instruction::CLD, AddrMode::IMP, 2},
    {"CMP", Instruction::CMP, AddrMode::ABY, 4},
    {"NOP", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"CMP", Instruction::CMP, AddrMode::ABX, 4},
    {"DEC", Instruction::DEC, AddrMode::ABX, 7},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"CPX", Instruction::CPX, AddrMode::IMM, 2},
    {"SBC", Instruction::SBC, AddrMode::IZX, 6},
    {"XXX", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"CPX", Instruction::CPX, AddrMode::ZPG, 3},
    {"SBC", Instruction::SBC, AddrMode::ZPG, 3},
    {"INC", Instruction::INC, AddrMode::ZPG, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 5},
    {"INX", Instruction::INX, AddrMode::IMP, 2},
    {"SBC", Instruction::SBC, AddrMode::IMM, 2},
    {"NOP", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::SBC, AddrMode::IMP, 2},
    {"CPX", Instruction::CPX, AddrMode::ABS, 4},
    {"SBC", Instruction::SBC, AddrMode::ABS, 4},
    {"INC", Instruction::INC, AddrMode::ABS, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"BEQ", Instruction::BEQ, AddrMode::REL, 2},
    {"SBC", Instruction::SBC, AddrMode::IZY, 5},
    {"XXX", Instruction::XXX, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 8},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"SBC", Instruction::SBC, AddrMode::ZPX, 4},
    {"INC", Instruction::INC, AddrMode::ZPX, 6},
    {"XXX", Instruction::XXX, AddrMode::IMP, 6},
    {"SED", Instruction::SED, AddrMode::IMP, 2},
    {"SBC", Instruction::SBC, AddrMode::ABY, 4},
    {"NOP", Instruction::NOP, AddrMode::IMP, 2},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
    {"XXX", Instruction::NOP, AddrMode::IMP, 4},
    {"SBC", Instruction::SBC, AddrMode::ABX, 4},
    {"INC", Instruction::INC, AddrMode::ABX, 7},
    {"XXX", Instruction::XXX, AddrMode::IMP, 7},
};
*/

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// Get the address of the actual stack pointer
// 0x0100 is the start of the stack.
// The SP register is an 8-bit register, so the CPU can effectively places the
// stack in the page at address 0x0100 to 0x01FF.
static inline uint16_t stack_addr(RegB rip) { return 0x0100 + rip; }

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
    DAT = 0x00;
    ABS = REL = 0x0000;

    // Initialize memory to nullptr
    disk = nullptr;
}

// Destructor
CPU::~CPU() {}

void CPU::Mount(const Disk &disk) { this->disk = (Disk *)&disk; }

// Reset the 6502 into a known state, hard-wired inside the CPU.
// - registers are set to 0x00
// - status register is cleared except for unused bit which remains at 1
// - an absolute address is read from location 0xFFFC which contains a second
//   address that the program counter is set to.
//   This allows the programmer to jump to a known and programmable location in
//   the memory to start executing from.
//   Typically the programmer would set the value at location 0xFFFC at compile
//   time.
void CPU::Reset() {

    // reset PC
    uint16_t lo = disk->ReadCPU(PC_ADDR);
    uint16_t hi = disk->ReadCPU(PC_ADDR + 1);
    PC = (hi << 8) | lo;

    // reset registers and flags
    RA = RX = RY = 0x00;
    SP = SP_INIT;
    RF.reg = 0b00100000;
}

// instuction functions
void CPU::LDA() {}
