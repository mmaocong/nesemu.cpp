// ============================================================================
// CPU of the NES i.e. the 6502 processor
//
// Reference:
//
// - https://www.nesdev.org/wiki/CPU
// - http://www.6502.org/tutorials/6502opcodes.html
// ============================================================================

#pragma once

#include "const.hpp"
#include "disk.hpp"

struct CPU {

    // implement flag register as a union of 1 byte and 1 struct
    union RegF {

        uint8_t reg;

        // implement each flag as a bit
        struct {
            uint8_t C : 1; // Carry
            uint8_t Z : 1; // Zero
            uint8_t I : 1; // Interrupt disable
            uint8_t D : 1; // Decimal mode
            uint8_t B : 1; // Break
            uint8_t U : 1; // Unused
            uint8_t V : 1; // Overflow
            uint8_t N : 1; // Negative
        };

        // set the negative flag
        inline void SetN(const Byte &f) { N = (f & (1 << 7)) != 0; }

        // set the zero flag
        inline void SetZ(const Byte &f) { Z = f == 0x00; }
    };

    // Initial stack pointer value
    static constexpr uint8_t SP_INIT = 0xFD;

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
    RegW TABS; // absolute address
    RegW TREL; // relative address
    RegB TDAT; // fetched data

    // storage
    Disk *disk; // disk

    // Constructor & Destructor
    CPU();
    ~CPU();

    // attach memory to the CPU
    void Mount(const Disk &disk);

    // ---------- Reset / Interrupt ----------

    void Reset();
    void IRQ();
    void NMI();

    // ---------- instruction functions ----------

    void BRK();
    void ORA();
    void XXX();
    void NOP();
    void ASL();
    void ALA(); // unofficial, same as ASL, only for IMP mode
    void PHP();
    void CLC();
    void JSR();
    void BIT();
    void AND();
    void ROL();
    void RLA(); // unofficial, same as ROL, only for IMP mode
    void PLP();
    void BMI();
    void SEC();
    void RTI();
    void EOR();
    void LSR();
    void LRA(); // unofficial, same as LSR, only for IMP mode
    void PHA();
    void JMP();
    void BVC();
    void CLI();
    void RTS();
    void ADC();
    void ROR();
    void RRA(); // unofficial, same as ROR, only for IMP mode
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

    // ---------- addressing mode functions ----------

    void IMP();
    void IMM();
    void ZPG();
    void ZPX();
    void ZPY();
    void REL();
    void ABS();
    void ABX();
    void AXP(); // unofficial, same as ABX, only for STA
    void ABY();
    void AYP(); // unofficial, same as ABY, only for STA
    void IND();
    void IZX();
    void IZY();
    void IYP(); // unofficial, same as IZY, only for STA

    // ---------- execute functions ----------

    void RunCycle();

    uint8_t RunInstr();

    void Print();

    void Exec(const uint16_t &, const size_t &);

    void BinToAsm(const Mem &, std::vector<std::string> &);
};
