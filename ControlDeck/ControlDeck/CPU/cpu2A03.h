#pragma once

#include "MemoryMapper.h"
#include "SystemComponents.h"
#include "InstructionDispatcher.h"

// http://users.telenet.be/kim1-6502/6502/hwman.html - general hardware manual source
namespace NES {
    struct DebugState {
        const OpCode *opCode;
        uint8_t addressingCycles;
        uint8_t branchCycles;

        uint8_t opCodeArgs[2];

        SystemBus systemBusBefore;
        Registers registersBefore;
        SystemBus systemBusAfter;
        Registers registersAfter;

        DMAData dmaBefore;
        DMAData dmaAfter;
        bool isDma{ false };
        void print();
    };

    /**
    * NTSC 6502 CPU 
    */
    class Cpu2a03{
    public:
        Cpu2a03() = default;
        Cpu2a03(MemoryMapper *memoryMapper, SystemRam *ram, SystemBus *systemBus, Registers *registers, DMAData *dmaData);

        DebugState processInstruction();

        void setPowerUpState();
        
        // hardware interrupt signals
        void reset();
        void setIrq();
        void setNmi();

        // -1 instructions means loop until powerdown/crash
        void cpuLoop(int numInstructions = -1);

        volatile bool powerDown{ false };
        MemoryMapper *memoryMapper;
        SystemRam *ram;
        SystemBus *systemBus;
        Registers *registers;
        DMAData *dmaData;

    protected:
        uint32_t cycle{ 0 };
        void waitForNextInstruction();

        // Fetch next op code or handle interrupt
        const OpCode *fetchOpCode();
    };
}

