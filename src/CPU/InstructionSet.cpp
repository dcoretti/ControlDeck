#include <ControlDeck/CPU/InstructionSet.h>

namespace NES {
    const char * addressingModeNames[] = {
        "Undefined",
        "Implicit",
        "Accumulator",
        "Immediate",
        "ZeroPage",
        "ZeroPageX",
        "ZeroPageY",
        "Relative",
        "Absolute",
        "AbsoluteX",
        "AbsoluteY",
        "Indirect",
        "XIndexedIndirect",
        "IndirectYIndexed",
    };
}