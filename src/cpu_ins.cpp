#include <iostream>

#include "cpu.hpp"

// PC address for reset / interrupt / NMI
static constexpr uint16_t ADDR_RES = 0xFFFC;
static constexpr uint16_t ADDR_IRQ = 0xFFFE;
static constexpr uint16_t ADDR_NMI = 0xFFFA;

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// Get the address of the actual stack pointer
// 0x0100 is the start of the stack.
// The SP register is an 8-bit register, so the CPU can effectively places the
// stack in the page at address 0x0100 to 0x01FF.
static inline uint16_t stack_addr(RegB rip) { return 0x0100 + rip; }

// ----------------------------------------------------------------------------
// NOP
// ----------------------------------------------------------------------------

// Capture illegal opcodes, functionally identical to NOP
void CPU::XXX() {}

// Instruction: No Operation
//
// TODO: potentially require an additional clock cycle
void CPU::NOP() {}

// Instruction: Test Bits
void CPU::BIT() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    // set flags
    RF.SetZ(val & RA);
    RF.SetN(val);
    RF.V = (val & (1 << 6)) > 0;
}

// ----------------------------------------------------------------------------
// Interrupts
// ----------------------------------------------------------------------------

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
    uint16_t lo = disk->ReadMBus(ADDR_RES);
    uint16_t hi = disk->ReadMBus(ADDR_RES + 1);
    PC = (hi << 8) | lo;

    // reset registers and flags
    RA = RX = RY = 0x00;
    SP = SP_INIT;
    TDAT = TABS = TREL = 0x0000;

    RF.reg = 0b00100000;
    cycles = 8;
    cyc_count = 0;
}

// Interrupt request.
//
// - Only happen if the "disable interrupt" flag is 0.
// - The current instruction is allowed to finish (which I facilitate by doing
// the whole thing when cycles == 0) and then the current program counter is
// stored on the stack. Then the current status register is stored on the stack.
// When the routine that services the interrupt has finished, the status
// register and program counter can be restored to how they where before it
// occurred. This is impemented by the "RTI" instruction. Once the IRQ
// has happened, in a similar way to a reset, a programmable address
// is read form hard coded location 0xFFFE, which is subsequently
// set to the program counter.
void CPU::IRQ() {
    if (RF.I == 1)
        return;
    // push program counter to stack
    // NOTE: little endian
    disk->WriteMBus(stack_addr(SP--), (PC >> 8) & 0x00FF);
    disk->WriteMBus(stack_addr(SP--), PC & 0x00FF);

    // set flags
    RF.B = 0;
    RF.U = 1;
    RF.I = 1;
    // push status register to stack
    disk->WriteMBus(stack_addr(SP--), RF.reg);
    // read new program counter location from fixed address
    uint16_t lo = disk->ReadMBus(ADDR_IRQ);
    uint16_t hi = disk->ReadMBus(ADDR_IRQ + 1);
    PC = (hi << 8) | lo;
    cycles = 7;
}

// Non-Maskable Interrupt, cannot be ignored.
//
// It behaves in exactly the same way as the IRQ, but reads the new program
// counter address form location 0xFFFA.
void CPU::NMI() {
    // push program counter to stack
    // NOTE: little endian
    disk->WriteMBus(stack_addr(SP--), (PC >> 8) & 0x00FF);
    disk->WriteMBus(stack_addr(SP--), PC & 0x00FF);

    // set flags
    RF.B = 0;
    RF.U = 1;
    RF.I = 1;
    // push status register to stack
    disk->WriteMBus(stack_addr(SP--), RF.reg);
    // read new program counter location from fixed address
    uint16_t lo = disk->ReadMBus(ADDR_NMI);
    uint16_t hi = disk->ReadMBus(ADDR_NMI + 1);
    PC = (hi << 8) | lo;
    cycles = 8;
}

// Program Sourced Interrupt
void CPU::BRK() {
    PC++;
    // push program counter to stack
    // NOTE: little endian
    disk->WriteMBus(stack_addr(SP--), (PC >> 8) & 0x00FF);
    disk->WriteMBus(stack_addr(SP--), PC & 0x00FF);

    // set flags
    RF.B = 1;

    // push status register to stack
    disk->WriteMBus(stack_addr(SP--), RF.reg);

    // set interrupt disable flag
    RF.B = 0;

    // read new program counter
    uint16_t lo = disk->ReadMBus(ADDR_IRQ);
    uint16_t hi = disk->ReadMBus(ADDR_IRQ + 1);
    PC = (hi << 8) | lo;
}

// Instruction: Return from Interrupt.
void CPU::RTI() {
    // pull status register from stack
    SP++;
    RF.reg = disk->ReadMBus(stack_addr(SP));

    // unset flags
    RF.B = 0;
    RF.U = 0;

    // pull program counter from stack
    // NOTE: little endian
    SP++;
    PC = disk->ReadMBus(stack_addr(SP));
    SP++;
    PC |= disk->ReadMBus(stack_addr(SP)) << 8;
}

// ----------------------------------------------------------------------------
// Bitwise
// ----------------------------------------------------------------------------

// Instruction: Bitwise Logic AND
//
// - A = A & M
// - Flags: N, Z
void CPU::AND() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    RA &= val;
    // set flags
    RF.SetZ(RA);
    RF.SetN(RA);
}

// Instruction: Bytewise Logical OR
//
// - A = A | M
// - Flags: N, Z
void CPU::ORA() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    RA |= val;

    // set flags
    RF.SetZ(RA);
    RF.SetN(RA);
}

// Instruction: Bitwise Logic XOR.
//
// - A = A xor M
// - Flags: N, Z
void CPU::EOR() {
    Byte val = disk->ReadMBus(TABS);
    RA ^= val;
    RF.SetZ(RA);
    RF.SetN(RA);
}

// ----------------------------------------------------------------------------
// Jump
// ----------------------------------------------------------------------------

// Instruction: Jump To Sub-Routine
//
// - PC -> stack
// - PC := address
void CPU::JSR() {
    PC--;

    // push program counter to stack
    // NOTE: little endian
    disk->WriteMBus(stack_addr(SP--), (PC >> 8) & 0x00FF);
    disk->WriteMBus(stack_addr(SP--), PC & 0x00FF);
    // set program counter
    PC = TABS;
}

// Instruction: Return from Sub-Routine.
void CPU::RTS() {
    // pull program counter from stack
    // NOTE: little endian
    SP++;
    PC = disk->ReadMBus(stack_addr(SP));
    SP++;
    PC |= disk->ReadMBus(stack_addr(SP)) << 8;
    // increment program counter so that it points to the next instruction
    PC++;
}

// Instruction: Jump To Location.
//
// - PC = address
void CPU::JMP() { PC = TABS; }

// ----------------------------------------------------------------------------
// Stack
// ----------------------------------------------------------------------------

// Instruction: Push Status Register to Stack
//
// - status -> stack
void CPU::PHP() {
    // save flags
    RF.B = 1;
    RF.U = 1;
    disk->WriteMBus(stack_addr(SP--), RF.reg);
    // reset flags
    RF.B = 0;
    RF.U = 0;
}

// Instruction: Pop Status Register off Stack.
//
// - Status <- stack
void CPU::PLP() {
    SP++;
    // pull status register from stack
    RF.reg = disk->ReadMBus(stack_addr(SP));
    // reset flags
    RF.U = 1;
    // NOTE: added to align with the nestest
    RF.B = 0;
}

// Instruction: Push Accumulator to Stack.
//
// - A -> stack
void CPU::PHA() {
    // push accumulator to stack
    disk->WriteMBus(stack_addr(SP--), RA);
}

// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
void CPU::PLA() {
    // pull accumulator from stack
    SP++;
    RA = disk->ReadMBus(stack_addr(SP));
    // set flags
    RF.SetZ(RA);
    RF.SetN(RA);
}

// ----------------------------------------------------------------------------
// Branching
// ----------------------------------------------------------------------------

// Instruction: Branch if Negative.
//
// - pc = address if N = 1
void CPU::BMI() {
    if (RF.N == 1) {
        // add extra cycle
        cycles++;
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Overflow Clear.
//
// - PC = address if V == 0
void CPU::BVC() {
    if (RF.V == 0) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Overflow Set.
//
// PC := address, if V = 1
void CPU::BVS() {
    if (RF.V == 1) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Carry Clear.
//
// - PC := address, if C == 0
void CPU::BCC() {
    if (RF.C == 0) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Carry Set.
//
// - PC := address, if C == 1
void CPU::BCS() {
    if (RF.C == 1) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Not Equal
//
// PC := address, if Z == 0
void CPU::BNE() {
    if (RF.Z == 0) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Equal
//
// - PC := address, if Z == 1
void CPU::BEQ() {
    if (RF.Z == 1) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// Instruction: Branch if Positive.
//
// - PC := address, if N == 0
void CPU::BPL() {
    if (RF.N == 0) {
        // add extra cycle
        cycles++;
        // set absolute address
        TABS = TREL + PC;
        // If page changed, add extra cycle
        if ((TABS & 0xFF00) != (PC & 0xFF00))
            cycles++;
        // set program counter
        PC = TABS;
    }
}

// ----------------------------------------------------------------------------
// Add & Subtract
// ----------------------------------------------------------------------------

// Instruction: Add with Carry.
//
// - add a value to the accumulator and a carry bit:
//   - A = A + M + C
// - Flags:
//   - C: set if the result is greater than 255
//   - Z: set if the result is 0
//   - N: negative flag, set if the MSB of the result is set
//   - V: need to be explained with a truth table:
//
//     | A  M  R | V | A^M | A^R |~(A^M) |
//     | 0  0  0 | 0 |  0  |  0  |   1   |
//     | 0  0  1 | 1 |  0  |  1  |   1   |
//     | 0  1  0 | 0 |  1  |  0  |   0   |
//     | 0  1  1 | 0 |  1  |  1  |   0   |
//     | 1  0  0 | 0 |  1  |  1  |   0   |
//     | 1  0  1 | 0 |  1  |  0  |   0   |
//     | 1  1  0 | 1 |  0  |  1  |   1   |
//     | 1  1  1 | 0 |  0  |  0  |   1   |
//
//     where R is the result of A + M + C
//
//     V = ~(A^M) & (A^R)
void CPU::ADC() {
    // fetch data from absolute address
    uint16_t val = disk->ReadMBus(TABS);
    // R = A + M + C
    uint16_t res = (uint16_t)RA + val + (uint16_t)RF.C;
    // set flags
    RF.C = res > 0xFF;
    RF.SetZ(res);
    RF.SetN(res);
    RF.V = ((~((uint16_t)RA ^ val) & ((uint16_t)RA ^ res)) & (1 << 7)) != 0;
    // store result in accumulator
    RA = res & 0x00FF;
}

// Instruction: Subtract with Borrow.
//
// - A = A - M - (1 - C) = A + (-M - 1) + C
// - Flags: C, V, N, Z
void CPU::SBC() {
    // fetch data from absolute address, and invert the bottom 8 bits
    // val := -M - 1
    uint16_t val = disk->ReadMBus(TABS) ^ 0x00FF;
    // R = A + (-M - 1) + C
    uint16_t res = (uint16_t)RA + val + (uint16_t)RF.C;
    // set flags
    RF.C = (res & 0xFF00) != 0;
    RF.SetZ(res);
    RF.SetN(res);
    RF.V = ((res ^ (uint16_t)RA) & (res ^ val) & (1 << 7)) != 0;
    // store result in accumulator
    RA = res & 0x00FF;
}

// ----------------------------------------------------------------------------
// Shift
// ----------------------------------------------------------------------------

// Instruction: Logical Shift Right.
void CPU::LSR() {
    // fetch data from absolute address
    uint16_t val = disk->ReadMBus(TABS);
    // set C flag
    RF.C = (val & 0x0001) != 0;
    // shift right
    val >>= 1;
    // set flags
    RF.SetZ(val);
    RF.SetN(val);
    // write data to absolute address
    disk->WriteMBus(TABS, val & 0x00FF);
}

// Instruction: Logical Shift Right (IMP mode).
// Same as LSR except that the result is stored in the accumulator
void CPU::LRA() {
    // fetch data from absolute address
    uint16_t val = disk->ReadMBus(TABS);
    // set C flag
    RF.C = (val & 0x0001) != 0;
    // shift right
    val >>= 1;
    // set flags
    RF.SetZ(val);
    RF.SetN(val);
    // store result in accumulator
    RA = val & 0x00FF;
}

// Instruction: Arithmetic Shift Left
//
// - A = C <- (A << 1) <- 0
// - Flags: N, Z, C
void CPU::ASL() {
    // fetch data from absolute address and shift left
    uint16_t val = disk->ReadMBus(TABS) << 1;
    // set flags
    RF.C = (val & 0xFF00) != 0;
    RF.SetZ(val);
    RF.SetN(val);
    // write data to absolute address
    disk->WriteMBus(TABS, val & 0x00FF);
}

// Instruction: Arithmetic Shift Left (IMP mode).
// Same as ASL except that the result is stored in the accumulator
void CPU::ALA() {
    // fetch data from absolute address and shift left
    uint16_t val = disk->ReadMBus(TABS) << 1;
    // set flags
    RF.C = (val & 0xFF00) != 0;
    RF.SetZ(val);
    RF.SetN(val);
    // store result in accumulator
    RA = val & 0x00FF;
}

// ----------------------------------------------------------------------------
// Rotate
// ----------------------------------------------------------------------------

// Instruction: Rotate Left
void CPU::ROL() {
    // fetch data from absolute address, rotate left, and set C flag
    // NOTE: C flag is set to the old bit 7
    uint16_t val = disk->ReadMBus(TABS) << 1 | (uint16_t)RF.C;
    // set flags
    RF.C = (val & 0xFF00) != 0;
    RF.SetZ(val);
    RF.SetN(val);
    // write data to absolute address
    disk->WriteMBus(TABS, val & 0x00FF);
}

// Instruction: Rotate Left (IMP mode).
// Same as ROL except that the result is stored in the accumulator
void CPU::RLA() {
    // f
    uint16_t val = disk->ReadMBus(TABS) << 1 | (uint16_t)RF.C;
    // set flags
    RF.C = (val & 0xFF00) != 0;
    RF.SetZ(val);
    RF.SetN(val);
    // store result in accumulator
    RA = val & 0x00FF;
}

// Instruction: ROtate Right.
//
// - The Carry is shifted into bit 7
// - The original bit 0 is shifted into the Carry.
void CPU::ROR() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    // rotate right
    uint16_t res = (val >> 1) | (RF.C << 7);
    // set flags
    RF.C = (val & 0x01) != 0;
    RF.SetZ(res);
    RF.SetN(res);
    // write data to absolute address
    disk->WriteMBus(TABS, res & 0x00FF);
}

// Instruction: ROtate Right (IMP mode).
// Same as ROR except that the result is stored in the accumulator
void CPU::RRA() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    // rotate right
    uint16_t res = (val >> 1) | (RF.C << 7);
    // set flags
    RF.C = (val & 0x01) != 0;
    RF.SetZ(res);
    RF.SetN(res);
    // store result in accumulator
    RA = res & 0x00FF;
}

// ----------------------------------------------------------------------------
// Save / Load Registers
// ----------------------------------------------------------------------------

// Instruction: Store Accumulator at Address.
//
// - M := A
void CPU::STA() { disk->WriteMBus(TABS, RA); }

// Instruction: Store Y Register at Address.
//
// - M = Y
void CPU::STY() { disk->WriteMBus(TABS, RY); }

// Instruction: Store X Register at Address.
//
// - M = X
void CPU::STX() { disk->WriteMBus(TABS, RX); }

// Instruction: Transfer X Register to Accumulator.
//
// - A = X
// - Flags: N, Z
void CPU::TXA() {
    RA = RX;
    RF.SetZ(RA);
    RF.SetN(RA);
}

// Instruction: Transfer Y Register to Accumulator.
//
// - A := Y
// - Flags: N, Z
void CPU::TYA() {
    RA = RY;
    RF.SetZ(RA);
    RF.SetN(RA);
}

// Instruction: Transfer X Register to Stack Pointer.
//
// SP := X
void CPU::TXS() { SP = RX; }

// Instruction: Load The Y Register.
//
// - Y := M
// - Flags: N, Z
void CPU::LDY() {
    RY = disk->ReadMBus(TABS);
    RF.SetZ(RY);
    RF.SetN(RY);
}

// Instruction: Load The Accumulator.
//
// - A := M
// - Flags: N, Z
void CPU::LDA() {
    RA = disk->ReadMBus(TABS);
    RF.SetZ(RA);
    RF.SetN(RA);
}

// Instruction: Load The X Register.
//
// - X := M
// - Flags: N, Z
void CPU::LDX() {
    RX = disk->ReadMBus(TABS);
    RF.SetZ(RX);
    RF.SetN(RX);
}

// Instruction: Transfer Accumulator to Y Register.
//
// - Y := A
// - Flags: N, Z
void CPU::TAY() {
    RY = RA;
    RF.SetZ(RY);
    RF.SetN(RY);
}

// Instruction: Transfer Accumulator to X Register
//
// - X := A
// - Flags: N, Z
void CPU::TAX() {
    RX = RA;
    RF.SetZ(RX);
    RF.SetN(RX);
}

// Instruction: Transfer Stack Pointer to X Register.
//
// - X := SP
// - Flags: N, Z
void CPU::TSX() {
    RX = SP;
    RF.SetZ(RX);
    RF.SetN(RX);
}

// ----------------------------------------------------------------------------
// Compare
// ----------------------------------------------------------------------------

// Instruction: Compare Y Register.
//
// - C := Y >= M
// - Z := (Y - M) == 0
// - N := (Y - M) & (1 << 7)
void CPU::CPY() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    uint16_t tmp = (uint16_t)RY - (uint16_t)val;
    // set flags
    RF.C = RY >= val;
    RF.SetZ(tmp);
    RF.SetN(tmp);
}

// Instruction: Compare Accumulator.
//
// - C := A >= M
// - Z := (A - M) == 0
// - N := (A - M) & (1 << 7)
void CPU::CMP() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    uint16_t tmp = (uint16_t)RA - (uint16_t)val;
    // set flags
    RF.C = RA >= val;
    RF.SetZ(tmp);
    RF.SetN(tmp);
}

// Instruction: Compare X Register.
//
// - C := X >= M
// - Z := (X - M) == 0
// - N := (X - M) & (1 << 7)
void CPU::CPX() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    uint16_t tmp = (uint16_t)RX - (uint16_t)val;
    // set flags
    RF.C = RX >= val;
    RF.SetZ(tmp);
    RF.SetN(tmp);
}

// ----------------------------------------------------------------------------
// Dec-Inc
// ----------------------------------------------------------------------------

// Instruction: Decrement Value at Memory Location.
//
// - M = M - 1
// - Flags: N, Z
void CPU::DEC() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    // decrement
    val--;
    // set flags
    RF.SetZ(val);
    RF.SetN(val);
    // write data to absolute address
    disk->WriteMBus(TABS, val);
}

// Instruction: Increment Value at Memory Location.
//
// - M := M + 1
// - Flags: N, Z
void CPU::INC() {
    // fetch data from absolute address
    Byte val = disk->ReadMBus(TABS);
    // increment
    val++;
    // set flags
    RF.SetZ(val);
    RF.SetN(val);
    // write data to absolute address
    disk->WriteMBus(TABS, val);
}

// Instruction: Decrement Y Register.
//
// - Y := Y - 1
// - Flags: N, Z
void CPU::DEY() {
    RY--;
    RF.SetZ(RY);
    RF.SetN(RY);
}

// Instruction: Increment Y Register.
//
// - Y := Y + 1
// - Flags: N, Z
void CPU::INY() {
    RY++;
    RF.SetZ(RY);
    RF.SetN(RY);
}

// Instruction: Decrement X Register.
//
// - X := X - 1
// - Flags: N, Z
void CPU::DEX() {
    RX--;
    RF.SetZ(RX);
    RF.SetN(RX);
}

// Instruction: Increment X Register.
//
// - X := X + 1
// - Flags: N, Z
void CPU::INX() {
    RX++;
    RF.SetZ(RX);
    RF.SetN(RX);
}

// ----------------------------------------------------------------------------
// Set / Clear Flags
// ----------------------------------------------------------------------------

// Instruction: Clear Carry Flag
//
// - C = 0
void CPU::CLC() { RF.C = 0; }

// Instruction: Clear Decimal Flag.
//
// D := 0
void CPU::CLD() { RF.D = 0; }

// Instruction: Clear Overflow Flag.
//
// - V := 0
void CPU::CLV() { RF.V = 0; }

// Instruction: Disable Interrupts / Clear Interrupt Flag.
//
// - I = 0
void CPU::CLI() { RF.I = 0; }

// Instruction: Set Carry Flag.
//
// - C = 1
void CPU::SEC() { RF.C = 1; }

// Instruction: Set Interrupt Flag / Enable Interrupts.
//
// - I := 1
void CPU::SEI() { RF.I = 1; }

// Instruction: Set Decimal Flag.
//
// - D := 1
void CPU::SED() { RF.D = 1; }
