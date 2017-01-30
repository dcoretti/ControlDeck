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


	// $4011 set to 0
	// reset preserves all registers except pc
	// reset vector is at FFFC,FFFD (usually ROM)
	// https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
    void Cpu2a03::reset() {

    }

	//https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
	void Cpu2a03::setPowerUpState() {
		registers->statusRegister = 0x34;
		registers->acc = 0;
		registers->x = 0;
		registers->y = 0;
		registers->stackPointer = 0xfd;
		registers->programCounter = 0xfffc;
	}

}