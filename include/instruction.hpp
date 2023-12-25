// ============================================================================
// Implement the 6502 instruction set.
//
// The instruction set (16 by 16 matrix) is implemented as a 1D array of
// 256 elements. Each element contains:
//  - the mnemonic
//  - the addressing mode
//  - the instruction function
//
// References:
//   - https://www.masswerk.at/6502/6502_instruction_set.html
// ============================================================================

#pragma once

#include <string>

enum AddrMode {
    IMP, // Implied, assume RA as operand, no data fetched
    IMM, // Immediate
    ZPG, // Zero Page
    ZPX, // Zero Page, X
    ZPY, // Zero Page, Y
    REL, // Relative
    ABS, // Absolute
    ABX, // Absolute, X
    ABY, // Absolute, Y
    IND, // Indirect
    IZX, // Indexed Indirect, X
    IZY, // Indirect Indexed, Y
};

struct Instruction {
    std::string name;
    void (*task)(void);
    AddrMode mode;
    uint8_t cycles;

    static void BRK();
    static void ORA();
    static void XXX();
    static void NOP();
    static void ASL();
    static void PHP();
    static void CLC();
    static void JSR();
    static void BIT();
    static void AND();
    static void ROL();
    static void PLP();
    static void BMI();
    static void SEC();
    static void RTI();
    static void EOR();
    static void LSR();
    static void PHA();
    static void JMP();
    static void BVC();
    static void CLI();
    static void RTS();
    static void ADC();
    static void ROR();
    static void PLA();
    static void BVS();
    static void SEI();
    static void STA();
    static void STY();
    static void STX();
    static void DEY();
    static void TXA();
    static void BCC();
    static void TYA();
    static void TXS();
    static void LDY();
    static void LDA();
    static void LDX();
    static void TAY();
    static void TAX();
    static void BCS();
    static void CLV();
    static void TSX();
    static void CPY();
    static void CMP();
    static void DEC();
    static void INY();
    static void DEX();
    static void BNE();
    static void CLD();
    static void CPX();
    static void SBC();
    static void INC();
    static void INX();
    static void BEQ();
    static void SED();
    static void BPL();
};
