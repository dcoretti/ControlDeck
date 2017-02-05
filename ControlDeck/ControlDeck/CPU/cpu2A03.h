#pragma once

#include "MemoryMapper.h"
#include "SystemComponents.h"
#include "InstructionDispatcher.h"

// http://users.telenet.be/kim1-6502/6502/hwman.html - general hardware manual source
namespace NES {
	struct DebugState {
		OpCode opCode;
		SystemBus systemBusBefore;
		Registers registersBefore;
		SystemBus systemBusAfter;
		Registers registersAfter;

		void print() {
			printf("[%s (%02x), addrMode: %s]:\n",
				instructionNames[opCode.instruction], opCode.opCode, addressingModeNames[opCode.addressingMode]);
			printf("B: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, p: $%02x, sp:$%02x, pc: $%04x}\n",
				systemBusBefore.addressBus, systemBusBefore.dataBus, systemBusBefore.read,
				registersBefore.acc, registersBefore.x, registersBefore.y, registersBefore.statusRegister, registersBefore.stackPointer, registersBefore.programCounter);
			printf("A: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, p: $%02x, sp: $%02x, pc: $%04x}\n",
				systemBusAfter.addressBus, systemBusAfter.dataBus, systemBusAfter.read,
				registersAfter.acc, registersAfter.x, registersAfter.y, registersAfter.statusRegister, registersAfter.stackPointer, registersAfter.programCounter);
		}

	};
    /**
    * NTSC 6502 CPU 
    */
    class Cpu2a03{
    public:
		Cpu2a03() = default;
		Cpu2a03(MemoryMapper *memoryMapper, SystemRam *ram, SystemBus *systemBus, Registers *registers);

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

    protected:
        void waitForNextInstruction();

        // Fetch next op code or handle interrupt
        OpCode fetchOpCode();
    };
}

