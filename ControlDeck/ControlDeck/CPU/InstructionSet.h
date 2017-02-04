#pragma once

namespace NES {
    // Sourced from http://e-tradition.net/bytes/6502/6502_instruction_set.html and
    // http://web.archive.org/web/20020820122913/http://www.obelisk.demon.co.uk/6502/reference.html#EOR
    enum Instruction {
        // Load store a single byte between memory and registers
        LDA,    // Load accumulator        N,Z
        LDX,    // Load x register        N,Z
        LDY,    // Load y register        N,Z
        STA,    // Store Accumulator
        STX,    // Store x register
        STY,    // Store y register

                // Transfer register and set negative (N), zero flag (Z) as necessary
        TAX,    // Transfer accumulator to X
        TAY,    // Transfer accumultor to Y
        TXA,    // Transfer x to accumulator
        TYA,    // Transfer y to accucmulator

                // Stack operations
        TSX,    // Transfer stack pointer to x
        TXS,    // Transfer x to stack pointer
        PHA,    // Push accumulator on stack
        PHP,    // Push processor status on stack
        PLA,    // Pull accumulator from stack
        PLP,    // Pull processor status from stack

                // logic operations
        AND,
        EOR,    // exclusive or
        ORA,    // logical inclusive or
        BIT,    // bit test

                // Arithmetic
        ADC,    // Add with carry    (carry bit is set to enable multibyte addition)
        SBC,    // Subtract with carry
        CMP,    // Compare accumulator
        CPX,    // Compare x register
        CPY,    // compare y register

                // increments/decriments
        INC,    // Increment memory location
        INX,    // Increment register x
        INY,    // Increment register y
        DEC,    // Decrement memory location
        DEX,    // Decrement register x
        DEY,    // Decrement register y

                // Shifts
        ASL,    // Arithmetic shift left
        LSR,    // Logical shift right
        ROL,    // Rotate left
        ROR,    // Rotate right

                // jump/call
        JMP,
        JSR,    // Jump to subroutine
        RTS,    // Return from subroutine

                // branching
        BCC,    // Branch if carry flag clear
        BCS,    // Branch if carry flag set
        BEQ,    // Branch if zero flag set
        BMI,    // Branch if neg flag set
        BNE,    // Branch if not equal flag clear
        BPL,    // Branch if negative flag clear
        BVC,    // Branch if overflow flag clear
        BVS,    // Branch if overflow flag set

                // Status flag changes
        CLC,    // clear carry flag
        CLD,    // Clear decimal mode flag      Not needed?
        CLI,    // Clear interrupt disable flag
        CLV,    // Clear overflow flag
        SEC,    // set carry flag
        SED,    // Set decimal mode flag        not needeD?
        SEI,    // Set interrupt disable flag

                // system functions
        BRK,    // Force interrupt
        NOP,
        RTI,    // Return from interrupt

        UNK,    // Unknown (not a real instruction)
    };

    extern const char* instructionNames[];
}