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

// address mode (0 - 15)
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
    AXP,     // unofficial, same as ABX, only for STA
    ABY,     // Absolute, Y
    AYP,     // unofficial, same as ABY, only for STA
    IND,     // Indirect
    IZX,     // Indexed Indirect, X
    IZY,     // Indirect Indexed, Y
    IYP,     // unofficial, same as IZY, only for STA
};

// instruction (0 - 72)
enum class Instruct : uint8_t {
    UNK = 0, // Unknown
    BRK,
    ORA,
    ORI, // unofficial, same as ORA, only for IMM mode
    XXX,
    NOP,
    ASL,
    ALA, // unofficial, same as ASL, only for IMP mode
    PHP,
    CLC,
    JSR,
    BIT,
    AND,
    ANI, // unofficial, same as AND, only for IMM mode
    ROL,
    RLA, // unofficial, same as ROL, only for IMP mode
    PLP,
    BMI,
    SEC,
    RTI,
    EOR,
    EOI, // unofficial, same as EOR, only for IMM mode
    LSR,
    LRA, // unofficial, same as LSR, only for IMP mode
    PHA,
    JMP,
    BVC,
    CLI,
    RTS,
    ADC,
    ADI, // unofficial, same as ADC, only for IMM mode
    ROR,
    RRA, // unofficial, same as ROR, only for IMP mode
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
    LYI, // unofficial, same as LDY, only for IMM mode
    LDA,
    LAI, // unofficial, same as LDA, only for IMM mode
    LDX,
    LXI, // unofficial, same as LDX, only for IMM mode
    TAY,
    TAX,
    BCS,
    CLV,
    TSX,
    CPY,
    CYI, // unofficial, same as CPY, only for IMM mode
    CMP,
    CMI, // unofficial, same as CMP, only for IMM mode
    DEC,
    INY,
    DEX,
    BNE,
    CLD,
    CPX,
    CXI, // unofficial, same as CPX, only for IMM mode
    SBC,
    SBI, // unofficial, same as SBC, only for IMM mode
    INC,
    INX,
    BEQ,
    SED,
    BPL,
};

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
    Disk *disk;

    // Initial program counter value
    size_t cyc_count;

    // ---------- logging ----------

    uint16_t addr; // address of the current instruction opcode
    AddrMode mode;
    Instruct instr;
    uint8_t n_param;
    uint8_t lhs; // operand for binary (LHS) or unary operation
    uint8_t rhs; // RHS operand (binary operation)

    // Constructor & Destructor
    CPU();
    ~CPU();

    // Read a whole instruction
    void Read();

    // attach memory to the CPU
    void Mount(const Disk &disk);

    // ---------- Reset / Interrupt ----------

    void Reset();
    void IRQ();
    void NMI();

    // ---------- instruction functions ----------

    void BRK();
    void ORA();
    void ORI(); // unofficial, same as ORA, only for IMM mode
    void XXX();
    void NOP();
    void ASL();
    void ALA(); // unofficial, same as ASL, only for IMP mode
    void PHP();
    void CLC();
    void JSR();
    void BIT();
    void AND();
    void ANI(); // unofficial, same as AND, only for IMM mode
    void ROL();
    void RLA(); // unofficial, same as ROL, only for IMP mode
    void PLP();
    void BMI();
    void SEC();
    void RTI();
    void EOR();
    void EOI(); // unofficial, same as EOR, only for IMM mode
    void LSR();
    void LRA(); // unofficial, same as LSR, only for IMP mode
    void PHA();
    void JMP();
    void BVC();
    void CLI();
    void RTS();
    void ADC();
    void ADI(); // unofficial, same as ADC, only for IMM mode
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
    void LYI(); // unofficial, same as LDY, only for IMM mode
    void LDA();
    void LAI(); // unofficial, same as LDA, only for IMM mode
    void LDX();
    void LXI(); // unofficial, same as LDX, only for IMM mode
    void TAY();
    void TAX();
    void BCS();
    void CLV();
    void TSX();
    void CPY();
    void CYI(); // unofficial, same as CPY, only for IMM mode
    void CMP();
    void CMI(); // unofficial, same as CMP, only for IMM mode
    void DEC();
    void INY();
    void DEX();
    void BNE();
    void CLD();
    void CPX();
    void CXI(); // unofficial, same as CPX, only for IMM mode
    void SBC();
    void SBI(); // unofficial, same as SBC, only for IMM mode
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

    void RunInstr();

    void Print();

    void BinToAsm(const Mem &, std::vector<std::string> &);
};
