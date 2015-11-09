#pragma once

namespace NES {
    enum AddressingMode {
        Undefined,
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
        XIndexedIndirect,	// indexed indirect
        IndirectYIndexed,	//indirect indexed
    };
}
