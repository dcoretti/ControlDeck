#include "InstructionDispatcher.h"

#include <cstdint>


namespace NES {
	// General Utilities 

	

	/*
	TXS,	// Push x to stack pointer
	PHA,	// Push accumulator on stack
	PHP,	// Push processor status on stack

	TSX,	// Pop stack pointer to x
	PLA,	// Pop accumulator from stack
	PLP,	// Pop processor status from stack
	*/

	void InstructionDispatcher::TXS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		systemBus.dataBus = registers.x;
		pushRegister(systemBus, registers, memoryMapper);
	}

	void InstructionDispatcher::PHA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		systemBus.dataBus = registers.acc;
		pushRegister(systemBus, registers, memoryMapper);
	}

	void InstructionDispatcher::PHP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		systemBus.dataBus = registers.statusRegister;
		pushRegister(systemBus, registers, memoryMapper);
	}


	void InstructionDispatcher::TSX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		popRegister(systemBus, registers, memoryMapper);
		registers.x = systemBus.dataBus;
	}

	void InstructionDispatcher::PLA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		popRegister(systemBus, registers, memoryMapper);
		registers.acc = systemBus.dataBus;
	}

	void InstructionDispatcher::PLP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		popRegister(systemBus, registers, memoryMapper);
		registers.statusRegister = systemBus.dataBus;
	}


	// Stack is pushed to from X register to memory location $0100 + Stack pointer offset (00-ff)
	// No overflow detection just like the NES
	void InstructionDispatcher::pushStackSetup(SystemBus &systemBus, Registers &registers) {
		systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
		registers.stackPointer += 8;
		systemBus.read = false;
	}

	// Data must be transfered to register X after read on stack instruction.  
	// TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
	void InstructionDispatcher::popStackSetup(SystemBus &systemBus, Registers &registers) {
		systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
		registers.stackPointer -= 8;
		systemBus.read = true;
	}

	void InstructionDispatcher::popRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		popStackSetup(systemBus, registers);
		memoryMapper.doMemoryOperation(systemBus);

		// Update processor status flags
		if (systemBus.dataBus == 0) {
			registers.flagSet(ProcessorStatus::ZeroFlag);
		}
		registers.setFlagIfNegative(systemBus.dataBus);
	}

	void InstructionDispatcher::pushRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
		pushStackSetup(systemBus, registers);
		memoryMapper.doMemoryOperation(systemBus);
	}
}