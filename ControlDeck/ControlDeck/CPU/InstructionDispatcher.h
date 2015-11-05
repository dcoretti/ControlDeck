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

		// internally executed instructions on memory data
		void ADC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void CMP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void EOR(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void LDY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void AND(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void CPX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void LDA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void ORA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void BIT(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void CPY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void LDX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
		void SBC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

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
