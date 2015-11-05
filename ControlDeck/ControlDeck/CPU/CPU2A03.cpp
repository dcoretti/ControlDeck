#include "cpu2A03.h"

#include "ReadWrite.h"
namespace NES {
	/**
	*	CPU
	*/
	Cpu2a03::Cpu2a03(MemoryMapper *memoryMapper): memoryMapper(memoryMapper) {

	}

	void Cpu2a03::fetchOpCode() {
		Cycle::Util::readDataFromProgramCounterSetup(systemBus, registers);
		memoryMapper->doMemoryOperation(systemBus);
	}

	void Cpu2a03::processInstruction() {
		fetchOpCode();
		dispatchOpCode(systemBus.dataBus);
	}

	void Cpu2a03::dispatchOpCode(uint8_t opCode) {
		
	}

	void Cpu2a03::waitForNextInstruction() {
		// implement per instruction wait.  
	}
}