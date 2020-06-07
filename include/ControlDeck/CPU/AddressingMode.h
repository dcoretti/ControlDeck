#pragma once
#include <cstdint>

namespace NES {
    enum AddressingMode {
        Undefined = 0,
        Implicit,
        Accumulator,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Relative,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        Indirect,
        XIndexedIndirect,    // indexed indirect
        IndirectYIndexed,    //indirect indexed
    };

    // for debug printing
    extern const char* addressingModeNames[];


    /**
    *   Table of how much to increment the program counter by for addressing mode operations.
    *   This doesn't include the op code fetch or further instruction operands
    */
    static const uint16_t addressingModeProgramCounterDelta[14] = {
        0,  // Undefined
        0,  // Implicit - no operand
        0,  // Accumulator

        1,  // Immediate - fetch operand constant byte 

        1,  // Zero Page - fetch operand byte (zero page adl)
        1,  // Zero page X
        1,  // Zero page Y

        1,  // Relative - fetch operand for branch (PC incremented by that operand if true)

        2,  // Absolute - fetch ADL,ADH operands
        2,  // Absolute X
        2,  // Absolute Y

        2,  // Indirect - fecth indirect addr (ADL,ADH) operands
        1,  // XIndexedIndirect - Fetch adl operand 
        1,  // IndirectYIndexed - fetch zero page (adl) operand
    };
}
