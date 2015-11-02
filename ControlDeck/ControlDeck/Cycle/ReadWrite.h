#pragma once

#include "../CPU/cpu2A03.h"
#include "../CPU/MemoryMapper.h"

namespace NES {
	// Single byte read from program counter
	void readDataFromProgramCounterSetup(SystemBus& systemBus, Registers &registers) {
		systemBus.addressBus = registers.programCounter++;
		systemBus.read = true;
	}
}