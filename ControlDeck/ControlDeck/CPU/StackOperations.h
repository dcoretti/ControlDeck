#pragma once

#include "cpu2A03.h"
#include "MemoryMapper.h"

namespace NES {
	namespace Instructions {
		// General Utilities 
		namespace {
			// Stack is pushed to from X register to memory location $0100 + Stack pointer offset (00-ff)
			// No overflow detection just like the NES
			void pushStackSetup(SystemBus &systemBus, Registers &registers) {
				systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
				registers.stackPointer += 8;
				systemBus.read = false;
			}

			// Data must be transfered to register X after read on stack instruction.  
			// TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
			void popStackSetup(SystemBus &systemBus, Registers &registers) {
				systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
				registers.stackPointer -= 8;
				systemBus.read = true;
			}

			void popRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
				popStackSetup(systemBus, registers);
				memoryMapper.doMemoryOperation(systemBus);

				// Update processor status flags
				if (systemBus.dataBus == 0) {
					registers.flagSet(ProcessorStatus::ZeroFlag);
				}
				registers.setFlagIfNegative(systemBus.dataBus);
			}

			void pushRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
				pushStackSetup(systemBus, registers);
				memoryMapper.doMemoryOperation(systemBus);
			}
		}

		/*
			TXS,	// Push x to stack pointer
			PHA,	// Push accumulator on stack
			PHP,	// Push processor status on stack

			TSX,	// Pop stack pointer to x
			PLA,	// Pop accumulator from stack
			PLP,	// Pop processor status from stack
		*/

		void TXS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
			systemBus.dataBus = registers.x;
			pushRegister(systemBus, registers, memoryMapper);
		}

		void PHA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
			systemBus.dataBus = registers.acc;
			pushRegister(systemBus, registers, memoryMapper);
		}

		void PHP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
			systemBus.dataBus = registers.statusRegister;
			pushRegister(systemBus, registers, memoryMapper);
		}


		void TSX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
			popRegister(systemBus, registers, memoryMapper);
			registers.x = systemBus.dataBus;
		}

		void PLA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
			popRegister(systemBus, registers, memoryMapper);
			registers.acc = systemBus.dataBus;
		}

		void PLP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
			popRegister(systemBus, registers, memoryMapper);
			registers.statusRegister = systemBus.dataBus;
		}
	}
}