#include "cpu2A03.h"

namespace NES {
	// Reference: http://e-tradition.net/bytes/6502/6502_instruction_set.html for flags altered by a given instruction
	// Program counter high byte
	inline uint8_t Registers::ah() {
		return (uint8_t)(acc & 0xF0);
	}

	// program counter low byte
	inline uint8_t Registers::al() {
		return (uint8_t)(acc & 0x0F);
	}

	// Program counter high byte
	inline uint8_t Registers::pch(const Registers &r) {
		return (uint8_t)(programCounter & 0xF);
	}

	// program counter low byte
	inline uint8_t Registers::pcl(const Registers &r) {
		return (uint8_t)(programCounter & 0x0F);
	}

	inline bool Registers::flagSet(ProcessorStatus flag) {
		return (statusRegister & (1 << flag)) != 0;
	}

	inline void Registers::setFlag(ProcessorStatus flag) {
		statusRegister |= (1 << flag);
	}

	inline void Registers::clearFlag(ProcessorStatus flag) {
		statusRegister &= ~(1 << flag);
	}

	inline void Registers::setFlagIfNegative(uint8_t val) {
		if (val >> 7 != 0) {
			setFlag(ProcessorStatus::NegativeFlag);
		}
	}
}