#pragma once

#include <cstdint>
#include "MemoryMapper.h"
#include "SystemComponents.h"

// http://users.telenet.be/kim1-6502/6502/hwman.html - general hardware manual source
namespace NES {
    /**
    * NTSC 6502 CPU 
    */
    class Cpu2a03{
    public:
        Cpu2a03(MemoryMapper *memoryMapper);

        void processInstruction();
    private:
        void dispatchOpCode(uint8_t opCode);
        void waitForNextInstruction();
        void fetchOpCode();


        // $4011 set to 0
        // reset preserves all registers except pc
        // reset vector is at FFFC,FFFD (usually ROM)
        void reset();

        SystemBus systemBus;
        SystemRam ram;
        Registers registers;
        MemoryMapper * memoryMapper;

    };
}

