#pragma once

#include <cstdint>

namespace NES {
	// Bit location of processor status flags 
	enum ProcessorStatus {
		CarryFlag = 0,			// Last operation caused overflow from bit 7.  
		ZeroFlag = 1,			// Result of last operation was 0
		InterruptDisable = 2,	// While set processor will not respond to interrupts from devices until cleared via CLI
		DecimalMode = 3,		// Not used (make arithmetic use BCD)
		BreakCommand = 4,		// Set after BRK and interrupt set to process
								// 5 is an ignored bit (always 1?) see http://www.fceux.com/web/help/fceux.html?6502CPU.html under "P Processor status"
		OverflowFlag = 6,		// Look at carry between bit 6 and 7, 7 and carry flag to determine if result switched sign via overflow
		NegativeFlag = 7		// If bit 7 set to 1 as a result of last operation
	};

	// Sources:  http://nesdev.com/NESDoc.pdf
	struct Registers {
		// Reference: http://e-tradition.net/bytes/6502/6502_instruction_set.html for flags altered by a given instruction
		// Program counter low byte
		inline uint8_t  pcl() {
			return (uint8_t)(programCounter & 0x0F);
		}

		// Program counter high byte
		inline uint8_t pch() {
			return (uint8_t)(programCounter >> 8);
		}

		// Status register utils
		// program counter low byte
		inline bool flagSet(ProcessorStatus flag) {
			return (statusRegister & (1 << flag)) != 0;
		}

		inline void setFlag(ProcessorStatus flag) {
			statusRegister |= (1 << flag);
		}

		inline void clearFlag(ProcessorStatus flag) {
			statusRegister &= ~(1 << flag);
		}

		inline void setFlagIfNegative(uint8_t val) {
			if (val >> 7 != 0) {
				setFlag(ProcessorStatus::NegativeFlag);
			}
		}


		// Commonly used for counters/offsets in memory accesss.  
		// Can be loaded/saved in memory, compared with memory values.
		// Can be used to get a copy of the stack pointer/change its value
		uint8_t x;

		// Commonly used for counters/offsets in memory accesss.  
		// Same as x but no ability to alter stack
		uint8_t y;

		// can be stored/retrieved from memory or stack
		uint8_t acc;

		// Register holds low 8 bits of the next free location on the stack
		// Stack located between $0100 and $01FF. 
		uint8_t stackPointer;

		// Program status flag register 
		uint8_t statusRegister;

		uint16_t programCounter;
	};

	/**
	*	System bus represents the state of the 16bit address lines and 8 bit data bus lines connecting the CPU to RAM or mapped memory.
	*   Each cycle involves a memory operation (read or a write).  The general usage is to set the address/data bus state depending on read/write
	*   and tell the memory mapper to handle the bus state.
	*   Read operations overwrite dataBus with the value in mapped memory for retrieval.
	*   See also http://users.telenet.be/kim1-6502/6502/hwman.html Appendix A for read/write ops, 1.3.1 for general concepts
	*/
	struct SystemBus {
		inline void setAdh(uint8_t val) {
			// clear high order bits
			addressBus &= 0x00ff;
			addressBus |= (val << 8);
		}

		inline void setAdl(uint8_t val) {
			uint16_t mask = 0;
			mask |= (uint16_t)val;
			addressBus |= mask;
		}

		inline void setAdhOnly(uint8_t val) {
			addressBus = val << 8;
		}

		inline void setAdlOnly(uint8_t val) {
			addressBus = val;
		}

		inline void setAddressBus(uint8_t adl, uint8_t adh) {
			addressBus = (adh << 8) + adl;
		}

		uint16_t addressBus;
		uint8_t dataBus;
		bool read;
	};

	struct SystemRam {
		// Actual System RAM
		static const size_t systemRAMBytes = 2048;
		uint8_t ram[systemRAMBytes];
	};
}