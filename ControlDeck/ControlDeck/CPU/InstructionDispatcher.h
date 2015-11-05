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
