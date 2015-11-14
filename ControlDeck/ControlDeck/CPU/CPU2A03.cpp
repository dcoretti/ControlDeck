#include "cpu2A03.h"
#include "InstructionDispatcher.h"
namespace NES {
    /**
    *	CPU
    */
    Cpu2a03::Cpu2a03(MemoryMapper *memoryMapper, SystemRam *ram, SystemBus *systemBus, Registers *registers) :
        memoryMapper(memoryMapper), ram(ram), registers(registers), systemBus(systemBus) {
    }

    void Cpu2a03::fetchOpCode() {
        systemBus->addressBus = registers->programCounter;
        systemBus->read = true;
        memoryMapper->doMemoryOperation(*systemBus);
        registers->programCounter++;
    }

    void Cpu2a03::processInstruction() {
        fetchOpCode();
        // This will need to return some information for the cpu to handle timing, etc.
        InstructionDispatcher::dispatchInstruction(*systemBus, *registers, *memoryMapper);
    }

    void Cpu2a03::waitForNextInstruction() {
        // implement per instruction wait.  
    }


    void Cpu2a03::reset() {

    }
}