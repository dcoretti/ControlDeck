#pragma once

#include "MemoryMapper.h"
#include "SystemComponents.h"
#include <unordered_map>

namespace NES {
	typedef void(*InstructionFnPtr)(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

	class InstructionDispatcher {
	public:
		InstructionDispatcher() {}
	private:


		// Single byte instructions
		void NOP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

		// shift (with acc addressing mode)
		void ASL(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void ROL(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void LSR(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

		// decrement
		void DEX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void DEY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);


		// accumulator transfer
		void TAX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void TYA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void TXA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void TAY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

		// Processor status register set/clear
		void CLC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void CLD(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void CLI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void CLV(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

		void INX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void SEC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void INY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void SED(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void SEI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

		// Stack operations
		void TXS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void PHA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void PHP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void TSX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void PLA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void PLP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);


		//utilities
		void pushStackSetup(SystemBus &systemBus, Registers &registers);
		void popStackSetup(SystemBus &systemBus, Registers &registers);
		void popRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void pushRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
	};
}
