#pragma once
#include "AddressingMode.h"
#include "SystemComponents.h"
#include "MemoryMapper.h"

namespace NES {
    /**
    *    Handler for instruction based parameter fetching and calculation of addresses.  Each instruction has an 
    *    implied mode of memory access that determines the number of operands to fetch after the op code, and what
    *    the final memory locations an instruction acts on.
    *
    *    Each method calculates the final absolute address in memory to be used by the instruction.  No 
    *   read or write is done on that address except for Immediate and Relative addressing, which puts the value on the data bus.
    *
    *    References for addressing implementation: http://nesdev.com/NESDoc.pdf Appendix E
    *   Cycle timing information sourced from http://users.telenet.be/kim1-6502/6502/hwman.html Appendix D
    */
    namespace AddressingModeHandler {
        // debug info about arguments for an op code
        struct OpCodeArgs {
            uint8_t args[2]{};
            // TODO also add indirectly loaded args?
        };

        OpCodeArgs handleAddressingMode(const AddressingMode addressingMode, SystemBus &bus, Registers &registers, MemoryMapper &memoryMapper);

        // TODO are page transitions an issue here (indirect address crossing page boundary to load JMP address?)
        // TODO use this instead of stack specific instructions which have implied addressing mode.  Make any instruction use addressing mode 
        //        handler to prepare for the next cycle by fetching op code operands??
        void getImmediateAddress(SystemBus &bus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getZeroPageAddress(SystemBus &bus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getXIndexedZeroPageAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getYIndexedZeroPageAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);

        // Put full 16 bit address on address bus.
        void getAbsoluateAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getXIndexedAbsoluteAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getYIndexedAbsoluteAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);

        void getIndirectAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getRelativeAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getXIndexedIndirectAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);
        void getIndirectYIndexedAddress(SystemBus &bus, Registers &registers,  MemoryMapper &memoryMapper, OpCodeArgs &args);


        // Utilities
        void  fetchIndirectAddressToBus(SystemBus& systemBus, MemoryMapper& memoryMapper);
        void  fetchAddressFromPCToBus(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args);
    }

}