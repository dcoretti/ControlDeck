#pragma once

#include "MemoryMapper.h"
#include "SystemComponents.h"
#include "InstructionDispatcher.h"

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

        void setPowerUpState();
        
        // hardware interrupt signals
        void reset();
        void setIrq();
        void setNmi();

    protected:
        void waitForNextInstruction();

        // Fetch next op code or handle interrupt
        OpCode fetchOpCode();

        SystemBus *systemBus;
        SystemRam *ram;
        Registers *registers;
        MemoryMapper * memoryMapper;
    };
}

