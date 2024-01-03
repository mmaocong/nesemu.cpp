#include "cpu.hpp"

// Addressing mode: Implied.
void CPU::IMP() { TDAT = RA; }

// Addressing mode: Immediate.
//
// - Expect the next byte to be used as a value
// - Prep the read address to point to the next byte
void CPU::IMM() { TABS = PC++; }

// Addressing mode: Zero Page.
//
// - Allows to absolutely address a location in the 1st 0xFF bytes of address
//   range.
// - This only requires one byte instead of the usual two.
void CPU::ZPG() {
    TABS = disk->ReadCPU(PC++);
    TABS &= 0x00FF;
}

// Addressing mode: Zero Page with X Register Offset.
//
// - The same as ZPG, but the X Register is added to the supplied single byte
//   address.
// - Useful for iterating through ranges within the first page.
void CPU::ZPX() {
    TABS = disk->ReadCPU(PC++) + RX;
    TABS &= 0x00FF;
}

// Addressing mode: Zero Page with Y Register Offset.
// - The same as ZPG, but the Y Register is added to the supplied single byte
//   address.
// - Useful for iterating through ranges within the first page.
void CPU::ZPY() {
    TABS = disk->ReadCPU(PC++) + RY;
    TABS &= 0x00FF;
}

// Addressing mode: Relative.
//
// - Exclusive to branch instructions.
// - The address must reside within -128 to +127 of the branch instruction,
//   i.e. CANNOT directly branch to any address in the addressable range.
void CPU::REL() {
    TREL = disk->ReadCPU(PC++);
    // Extend the sign if the value is negative
    if (TREL & (1 << 7))
        TREL |= 0xFF00;
}

// Addressing mode: Absolute.
//
// A full 16-bit address is loaded and used.
void CPU::ABS() {
    uint16_t lo = disk->ReadCPU(PC++);
    uint16_t hi = disk->ReadCPU(PC++);
    TABS = (hi << 8) | lo;
}

// Addressing mode: Absolute with X Register Offset.
//
// - Same as ABS, but the X Register is added to the supplied two byte address.
// - If the resulting address changes the page, an additional clock cycle is
//   required
// - Corresponding Instruction: ORA, AND, EOR, ADC, LDY, LDA, CMP, SBC
void CPU::ABX() {
    uint16_t lo = disk->ReadCPU(PC++);
    uint16_t hi = disk->ReadCPU(PC++);
    TABS = (hi << 8) | lo;
    TABS += RX;

    if ((TABS & 0xFF00) != (hi << 8))
        cycles++;
}

// Addressing mode: Absolute with X Register Offset (Plain Version).
//
// Same as ABX, but only used by LDX, and does not require the additional clock
//
// Corresponding Instruction: ASL, ROL, LSR, ROR, STA, DEC, INC
void CPU::AXP() {
    uint16_t lo = disk->ReadCPU(PC++);
    uint16_t hi = disk->ReadCPU(PC++);
    TABS = (hi << 8) | lo;
    TABS += RX;
}

// Addressing mode: Absolute with Y Register Offset.
//
// - Same as ABS, but the Y Register is added to the supplied two byte address.
// - If the resulting address changes the page, an additional clock cycle is
//   required
// - Corresponding Instruction: ORA, AND, EOR, ADC, LDA, LDX, CMP, SBC
void CPU::ABY() {
    uint16_t lo = disk->ReadCPU(PC++);
    uint16_t hi = disk->ReadCPU(PC++);
    TABS = (hi << 8) | lo;
    TABS += RY;

    if ((TABS & 0xFF00) != (hi << 8))
        cycles++;
}

// Addressing mode: Absolute with Y Register Offset (Plain Version).
//
// Same as ABY, but only used by STA, and does not require the additional clock
void CPU::AYP() {
    uint16_t lo = disk->ReadCPU(PC++);
    uint16_t hi = disk->ReadCPU(PC++);
    TABS = (hi << 8) | lo;
    TABS += RY;
}

// Addressing mode: Indirect.
//
// - The supplied 16-bit address is read to get the actual 16-bit address.
// - NOTE: BUG in the hardware.
//   If the low byte of the supplied address is 0xFF, then to read the high
//   byte of the actual address we need to cross a page boundary.
//   This does NOT actually work on the chip as designed, instead it wraps
//   back around in the same page, yielding an invalid actual address
void CPU::IND() {
    uint16_t lo = disk->ReadCPU(PC++);
    uint16_t hi = disk->ReadCPU(PC++);
    uint16_t ptr = (hi << 8) | lo;

    if (lo == 0x00FF)
        // simulate page boundary hardware bug
        TABS = (disk->ReadCPU(ptr & 0xFF00) << 8) | disk->ReadCPU(ptr);
    else
        // normal operation
        TABS = (disk->ReadCPU(ptr + 1) << 8) | disk->ReadCPU(ptr);
}

// Addressing mode: Indexed Indirect with X Register Offset.
//
// - The supplied 8-bit address is offset by X Register to index a location in
//   page 0x00.
// - The actual 16-bit address is read from this location
void CPU::IZX() {
    uint16_t ptr = disk->ReadCPU(PC++);
    uint8_t lo = disk->ReadCPU((ptr + (uint16_t)RX) & 0x00FF);
    uint8_t hi = disk->ReadCPU((ptr + (uint16_t)RX + 1) & 0x00FF);
    TABS = (hi << 8) | lo;
}

// Addressing mode: Indirect Indexed with Y Register Offset.
//
// - The supplied 8-bit address indexes a location in page 0x00.
// - From here the actual 16-bit address is read, and the Y Register is added
//   to it to offset it.
// - If the offset causes a change in page then an additional clock cycle is
//   required.
// - Corresponding Instruction: ORA, AND, EOR, ADC, LDA, CMP, SBC
void CPU::IZY() {
    uint16_t ptr = disk->ReadCPU(PC++);
    uint8_t lo = disk->ReadCPU(ptr & 0x00FF);
    uint8_t hi = disk->ReadCPU((ptr + 1) & 0x00FF);
    TABS = ((hi << 8) | lo) + (uint16_t)RY;

    if ((TABS & 0xFF00) != (hi << 8))
        cycles++;
}

// Addressing mode: Indexed Indirect with Y Register Offset (Plain Version).
//
// Same as IZY, but only used by STA, and does not require the additional clock
void CPU::IYP() {
    uint16_t ptr = disk->ReadCPU(PC++);
    uint8_t lo = disk->ReadCPU(ptr & 0x00FF);
    uint8_t hi = disk->ReadCPU((ptr + 1) & 0x00FF);
    TABS = ((hi << 8) | lo) + (uint16_t)RY;
}
