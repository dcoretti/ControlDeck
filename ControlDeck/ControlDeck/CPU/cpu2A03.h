#pragma once

#include "MemoryMapper.h"
#include "SystemComponents.h"

// http://users.telenet.be/kim1-6502/6502/hwman.html - general hardware manual source
namespace NES {
    /**
    * NTSC 6502 CPU 
    */
    class Cpu2a03{
    public:
        Cpu2a03() {}
        Cpu2a03(MemoryMapper *memoryMapper, SystemRam *ram, SystemBus *systemBus, Registers *registers);

        void processInstruction();

        // $4011 set to 0
        // reset preserves all registers except pc
        // reset vector is at FFFC,FFFD (usually ROM)
        void reset();
    protected:
        void waitForNextInstruction();
        void fetchOpCode();

        SystemBus *systemBus;
        SystemRam *ram;
        Registers *registers;
        MemoryMapper * memoryMapper;
    };
}

