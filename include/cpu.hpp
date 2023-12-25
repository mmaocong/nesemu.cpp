// ============================================================================
// CPU of the NES i.e. the 6502 processor
// ============================================================================

#pragma once

#include "const.hpp"
#include "disk.hpp"

using RegB = uint8_t;  // byte-sized register
using RegW = uint16_t; // word-sized register

// implement flag register as a union of 1 byte and 1 struct
union RegF {

    uint8_t reg;

    // implement each flag as a bit
    struct Flags {
        uint8_t C : 1; // Carry
        uint8_t Z : 1; // Zero
        uint8_t I : 1; // Interrupt disable
        uint8_t D : 1; // Decimal mode
        uint8_t B : 1; // Break
        uint8_t U : 1; // Unused
        uint8_t V : 1; // Overflow
        uint8_t N : 1; // Negative
    } flags;

    // get a flag
    uint8_t get(const char &flag) {
        switch (flag) {
        case 'C':
            return flags.C;
        case 'Z':
            return flags.Z;
        case 'I':
            return flags.I;
        case 'D':
            return flags.D;
        case 'B':
            return flags.B;
        case 'U':
            return flags.U;
        case 'V':
            return flags.V;
        case 'N':
            return flags.N;
        default:
            return 0;
        }
    }

    // toggle a flag
    void set(const char &flag) {
        switch (flag) {
        case 'C':
            flags.C = !flags.C;
            break;
        case 'Z':
            flags.Z = !flags.Z;
            break;
        case 'I':
            flags.I = !flags.I;
            break;
        case 'D':
            flags.D = !flags.D;
            break;
        case 'B':
            flags.B = !flags.B;
            break;
        case 'U':
            flags.U = !flags.U;
            break;
        case 'V':
            flags.V = !flags.V;
            break;
        case 'N':
            flags.N = !flags.N;
            break;
        default:
            break;
        }
    }
};

struct CPU {

    // Initial stack pointer value
    static constexpr uint8_t SP_INIT = 0xFD;
    // Initial program counter address
    static constexpr uint16_t PC_ADDR = 0xFFFC;

    // Registers
    // store results and the program control flow
    RegW PC; // Program counter
    RegF RF; // Flag register
    RegB RA; // Accumulator
    RegB RX; // X index
    RegB RY; // Y index
    RegB SP; // Stack pointer

    // clock cycles for synchronization
    uint8_t cycles;

    // Temporary Registers
    // store the fetched data and addressess
    RegW ABS; // absolute address
    RegW REL; // relative address
    RegB DAT; // fetched data

    // storage
    Disk *disk; // disk

    // Constructor & Destructor
    CPU();
    ~CPU();

    // attach memory to the CPU
    void Mount(const Disk &disk);

    // Reset the CPU
    void Reset();

    // instruction functions
    void BRK();
    void ORA();
    void XXX();
    void NOP();
    void ASL();
    void PHP();
    void CLC();
    void JSR();
    void BIT();
    void AND();
    void ROL();
    void PLP();
    void BMI();
    void SEC();
    void RTI();
    void EOR();
    void LSR();
    void PHA();
    void JMP();
    void BVC();
    void CLI();
    void RTS();
    void ADC();
    void ROR();
    void PLA();
    void BVS();
    void SEI();
    void STA();
    void STY();
    void STX();
    void DEY();
    void TXA();
    void BCC();
    void TYA();
    void TXS();
    void LDY();
    void LDA();
    void LDX();
    void TAY();
    void TAX();
    void BCS();
    void CLV();
    void TSX();
    void CPY();
    void CMP();
    void DEC();
    void INY();
    void DEX();
    void BNE();
    void CLD();
    void CPX();
    void SBC();
    void INC();
    void INX();
    void BEQ();
    void SED();
    void BPL();
};
