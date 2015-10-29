#pragma once

#include <cstdint>
namespace NES {
	enum ProcessorStatus {
		CarryFlag,			// Last operation caused overflow from bit 7.  
		ZeroFlag,			// Result of last operation was 0
		InterruptDisable,	// While set processor will not respond to interrupts from devices until cleared via CLI
		DecimalMode,		// Not used (make arithmetic use BCD)
		BreakCommand,		// Set after BRK and interrupt set to process
		OverflowFlag,		// Look at carry between bit 6 and 7, 7 and carry flag to determine if result switched sign via overflow
		NegativeFlag		// If bit 7 set to 1 as a result of last operation
	};

	enum Instruction {

		// Load store a single byte between memory and registers
		LDA,	// Load accumulator		N,Z
		LDX,	// Load x register		N,Z
		LDY,	// Load y register		N,Z
		STA,	// Store Accumulator
		STX,	// Store x register
		STY,	// Store y register

		// Transfer register and set negative (N), zero flag (Z) as necessary
		TAX,	// Transfer accumulator to X
		TAY,	// Transfer accumultor to Y
		TXA,	// Transfer x to accumulator
		TYA,	// Transfer y to accucmulator

		// Stack operations
		TSX,	// Transfer stack pointer to x
		TXS,	// Transfer x to stack pointer
		PHA,	// Push accumulator on stacl
		PHP,	// Push processor status on stack
		PLA,	// Pull accumulator from stack
		PLP,	// Pull processor status from stack

		// logic operations
		AND,
		EOR,	// exclusive or
		ORA,	// logical inclusive or
		BIT,	// bit test

		// Arithmetic
		ADC,	// Add with carry	(carry bit is set to enable multibyte addition)
		SBC,	// Subtract with carry
		CMP,	// Compare accumulator
		CPX,	// Compare x register
		CPY,	// compare y register

		// increments/decriments
		INC,	// Increment memory location
		INX,	// Increment register x
		INY,	// Increment register y
		DEC,	// Decrement memory location
		DEX,	// Decrement register x
		DEY,	// Decrement register y

		// Shifts
		ASL,	// Arithmetic shift left
		LSR,	// Logical shift right
		ROL,	// Rotate left
		ROR,	// Rotate right

		// jump/call
		JMP,	
		JSR,	// Jump to subroutine
		RTS,	// Return from subroutine

		// branching
		BCC,	// Branch if carry flag clear
		BCS,	// Branch if carry flag set
		BEQ,	// Branch if zero flag set
		BMI,	// Branch if neg flag set
		BNE,	// Branch if not equal flag clear
		BPL,	// Branch if negative flag clear
		BVC,	// Branch if overflow flag clear
		BVS,	// Branch if overflow flag set

		// Status flag changes
		CLC,	// clear carry flag
		CLD,	// Clear decimal mode flag      Not needed?
		CLI,	// Clear interrupt disable flag
		CLV,	// Clear overflow flag
		SEC,	// set carry flag
		SED,	// Set decimal mode flag		not needeD?
		SEI,	// Set interrupt disable flag

		// system functions
		BRK,	// Force interrupt
		NOP,
		RTI,	// Return from interrupt

		UNK,	// Unknown (not a real instruction)
	};

	enum AddressingMode {
		Undefined,
		Implicit,
		Accumulator,
		Immediate,
		ZeroPage,
		ZeroPageX,
		ZeroPageY,
		Relative,
		Absolute,
		AbsoluteX,
		AbsoluteY,
		Indirect,
		XIndexedIndirect,	// indexed indirect
		IndirectYIndexed,	//indirect indexed
	};

	struct OpCode {
		uint8_t opCode;
		uint8_t bytes;
		// All instructions are at least 2 cycles long.  First cycle is always to increment and select the program counter as the next address to appear on the bus
		uint8_t cycles;
		uint8_t pageCycles;	// 0,1,2 1 for page boundary crossed, 2 for branch sending to different page
		Instruction instruction;
		AddressingMode addressingMode;
	};
	
	

	OpCode opCodes[] = {
		{ 0x00, 0, 0, 0, Instruction::BRK, AddressingMode::Implicit },	
		{ 0x01, 0, 0, 0, Instruction::ORA, AddressingMode::XIndexedIndirect },
		{ 0x02, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },		
		{ 0x03, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x04, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x05, 0, 0, 0, Instruction::ORA, AddressingMode::ZeroPage },
		{ 0x06, 0, 0, 0, Instruction::ASL, AddressingMode::ZeroPage },
		{ 0x07, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x08, 0, 0, 0, Instruction::PHP, AddressingMode::Implicit },
		{ 0x09, 0, 0, 0, Instruction::ORA, AddressingMode::Immediate },
		{ 0x0a, 0, 0, 0, Instruction::ASL, AddressingMode::Accumulator },
		{ 0x0b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x0c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x0d, 0, 0, 0, Instruction::ORA, AddressingMode::Absolute },
		{ 0x0e, 0, 0, 0, Instruction::ASL, AddressingMode::Absolute },
		{ 0x0f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x10, 0, 0, 0, Instruction::BPL, AddressingMode::Relative },
		{ 0x11, 0, 0, 0, Instruction::ORA, AddressingMode::IndirectYIndexed },
		{ 0x12, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x13, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x14, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x15, 0, 0, 0, Instruction::ORA, AddressingMode::ZeroPageX },
		{ 0x16, 0, 0, 0, Instruction::ASL, AddressingMode::ZeroPageX },
		{ 0x17, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x18, 0, 0, 0, Instruction::CLC, AddressingMode::Implicit },
		{ 0x19, 0, 0, 0, Instruction::ORA, AddressingMode::AbsoluteY },
		{ 0x1a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x1b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x1c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x1d, 0, 0, 0, Instruction::ORA, AddressingMode::AbsoluteX },
		{ 0x1e, 0, 0, 0, Instruction::ASL, AddressingMode::AbsoluteX },
		{ 0x1f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x20, 0, 0, 0, Instruction::JSR, AddressingMode::Absolute },
		{ 0x21, 0, 0, 0, Instruction::AND, AddressingMode::XIndexedIndirect },
		{ 0x22, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x23, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x24, 0, 0, 0, Instruction::BIT, AddressingMode::ZeroPage },
		{ 0x25, 0, 0, 0, Instruction::AND, AddressingMode::ZeroPage },
		{ 0x26, 0, 0, 0, Instruction::ROL, AddressingMode::ZeroPage },
		{ 0x27, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x28, 0, 0, 0, Instruction::PLP, AddressingMode::Implicit },
		{ 0x29, 0, 0, 0, Instruction::AND, AddressingMode::Immediate },
		{ 0x2a, 0, 0, 0, Instruction::ROL, AddressingMode::Accumulator },
		{ 0x2b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x2c, 0, 0, 0, Instruction::BIT, AddressingMode::Absolute },
		{ 0x2d, 0, 0, 0, Instruction::AND, AddressingMode::Absolute },
		{ 0x2e, 0, 0, 0, Instruction::ROL, AddressingMode::Absolute },
		{ 0x2f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x30, 0, 0, 0, Instruction::BMI, AddressingMode::Relative },
		{ 0x31, 0, 0, 0, Instruction::AND, AddressingMode::IndirectYIndexed },
		{ 0x32, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x33, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x34, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x35, 0, 0, 0, Instruction::AND, AddressingMode::ZeroPageX },
		{ 0x36, 0, 0, 0, Instruction::ROL, AddressingMode::ZeroPageX },
		{ 0x37, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x38, 0, 0, 0, Instruction::SEC, AddressingMode::Implicit },
		{ 0x39, 0, 0, 0, Instruction::AND, AddressingMode::AbsoluteY },
		{ 0x3a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x3b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x3c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x3d, 0, 0, 0, Instruction::AND, AddressingMode::AbsoluteX },
		{ 0x3e, 0, 0, 0, Instruction::ROL, AddressingMode::AbsoluteX },
		{ 0x3f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x40, 0, 0, 0, Instruction::RTI, AddressingMode::Implicit },
		{ 0x41, 0, 0, 0, Instruction::EOR, AddressingMode::XIndexedIndirect },
		{ 0x42, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x43, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x44, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x45, 0, 0, 0, Instruction::EOR, AddressingMode::ZeroPage },
		{ 0x46, 0, 0, 0, Instruction::LSR, AddressingMode::ZeroPage },
		{ 0x47, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x48, 0, 0, 0, Instruction::PHA, AddressingMode::Implicit },
		{ 0x49, 0, 0, 0, Instruction::EOR, AddressingMode::Immediate },
		{ 0x4a, 0, 0, 0, Instruction::LSR, AddressingMode::Accumulator },
		{ 0x4b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x4c, 0, 0, 0, Instruction::JMP, AddressingMode::Absolute },
		{ 0x4d, 0, 0, 0, Instruction::EOR, AddressingMode::Absolute },
		{ 0x4e, 0, 0, 0, Instruction::LSR, AddressingMode::Absolute },
		{ 0x4f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x50, 0, 0, 0, Instruction::BVC, AddressingMode::Relative },
		{ 0x51, 0, 0, 0, Instruction::EOR, AddressingMode::IndirectYIndexed },
		{ 0x52, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x53, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x54, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x55, 0, 0, 0, Instruction::EOR, AddressingMode::ZeroPageX },
		{ 0x56, 0, 0, 0, Instruction::LSR, AddressingMode::ZeroPageX },
		{ 0x57, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x58, 0, 0, 0, Instruction::CLI, AddressingMode::Implicit },
		{ 0x59, 0, 0, 0, Instruction::EOR, AddressingMode::AbsoluteY },
		{ 0x5a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x5b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x5c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x5d, 0, 0, 0, Instruction::EOR, AddressingMode::AbsoluteX },
		{ 0x5e, 0, 0, 0, Instruction::LSR, AddressingMode::AbsoluteX },
		{ 0x5f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x60, 0, 0, 0, Instruction::RTS, AddressingMode::Implicit },
		{ 0x61, 0, 0, 0, Instruction::ADC, AddressingMode::Indirect },
		{ 0x62, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x63, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x64, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x65, 0, 0, 0, Instruction::ADC, AddressingMode::ZeroPage },
		{ 0x66, 0, 0, 0, Instruction::ROR, AddressingMode::ZeroPage },
		{ 0x67, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x68, 0, 0, 0, Instruction::PLA, AddressingMode::Implicit },
		{ 0x69, 0, 0, 0, Instruction::ADC, AddressingMode::Immediate },
		{ 0x6a, 0, 0, 0, Instruction::ROR, AddressingMode::Accumulator },
		{ 0x6b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x6c, 0, 0, 0, Instruction::JMP, AddressingMode::Indirect },
		{ 0x6d, 0, 0, 0, Instruction::ADC, AddressingMode::Absolute },
		{ 0x6e, 0, 0, 0, Instruction::ROR, AddressingMode::Absolute },
		{ 0x6f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x70, 0, 0, 0, Instruction::BVS, AddressingMode::Relative },
		{ 0x71, 0, 0, 0, Instruction::ADC, AddressingMode::IndirectYIndexed },
		{ 0x72, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x73, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x74, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x75, 0, 0, 0, Instruction::ADC, AddressingMode::ZeroPageX },
		{ 0x76, 0, 0, 0, Instruction::ROR, AddressingMode::ZeroPageX },
		{ 0x77, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x78, 0, 0, 0, Instruction::SEI, AddressingMode::Implicit },
		{ 0x79, 0, 0, 0, Instruction::ADC, AddressingMode::AbsoluteY },
		{ 0x7a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x7b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x7c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x7d, 0, 0, 0, Instruction::ADC, AddressingMode::AbsoluteX },
		{ 0x7e, 0, 0, 0, Instruction::ROR, AddressingMode::AbsoluteX },
		{ 0x7f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x80, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x81, 0, 0, 0, Instruction::STA, AddressingMode::Indirect },
		{ 0x82, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x83, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x84, 0, 0, 0, Instruction::STY, AddressingMode::ZeroPage },
		{ 0x85, 0, 0, 0, Instruction::STA, AddressingMode::ZeroPage },
		{ 0x86, 0, 0, 0, Instruction::STX, AddressingMode::ZeroPage },
		{ 0x87, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x88, 0, 0, 0, Instruction::DEY, AddressingMode::Implicit },
		{ 0x89, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x8a, 0, 0, 0, Instruction::TXA, AddressingMode::Implicit },
		{ 0x8b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x8c, 0, 0, 0, Instruction::STY, AddressingMode::Absolute },
		{ 0x8d, 0, 0, 0, Instruction::STA, AddressingMode::Absolute },
		{ 0x8e, 0, 0, 0, Instruction::STX, AddressingMode::Absolute },
		{ 0x8f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0x90, 0, 0, 0, Instruction::BCC, AddressingMode::Relative },
		{ 0x91, 0, 0, 0, Instruction::STA, AddressingMode::IndirectYIndexed },
		{ 0x92, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x93, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x94, 0, 0, 0, Instruction::STY, AddressingMode::ZeroPageX },
		{ 0x95, 0, 0, 0, Instruction::STA, AddressingMode::ZeroPageX },
		{ 0x96, 0, 0, 0, Instruction::STX, AddressingMode::ZeroPageY },
		{ 0x97, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x98, 0, 0, 0, Instruction::TYA, AddressingMode::Implicit },
		{ 0x99, 0, 0, 0, Instruction::STA, AddressingMode::AbsoluteY },
		{ 0x9a, 0, 0, 0, Instruction::TXS, AddressingMode::Implicit },
		{ 0x9b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x9c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x9d, 0, 0, 0, Instruction::STA, AddressingMode::AbsoluteX },
		{ 0x9e, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0x9f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0xa0, 0, 0, 0, Instruction::LDY, AddressingMode::Immediate },
		{ 0xa1, 0, 0, 0, Instruction::LDA, AddressingMode::XIndexedIndirect },
		{ 0xa2, 0, 0, 0, Instruction::LDX, AddressingMode::Immediate },
		{ 0xa3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xa4, 0, 0, 0, Instruction::LDY, AddressingMode::ZeroPage },
		{ 0xa5, 0, 0, 0, Instruction::LDA, AddressingMode::ZeroPage },
		{ 0xa6, 0, 0, 0, Instruction::LDX, AddressingMode::ZeroPage },
		{ 0xa7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xa8, 0, 0, 0, Instruction::TAY, AddressingMode::Implicit },
		{ 0xa9, 0, 0, 0, Instruction::LDA, AddressingMode::Immediate },
		{ 0xaa, 0, 0, 0, Instruction::TAX, AddressingMode::Implicit },
		{ 0xab, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xac, 0, 0, 0, Instruction::LDY, AddressingMode::Absolute },
		{ 0xad, 0, 0, 0, Instruction::LDA, AddressingMode::Absolute },
		{ 0xae, 0, 0, 0, Instruction::LDX, AddressingMode::Absolute },
		{ 0xaf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0xb0, 0, 0, 0, Instruction::BCS, AddressingMode::Relative },
		{ 0xb1, 0, 0, 0, Instruction::LDA, AddressingMode::IndirectYIndexed },
		{ 0xb2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xb3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xb4, 0, 0, 0, Instruction::LDY, AddressingMode::ZeroPageX },
		{ 0xb5, 0, 0, 0, Instruction::LDA, AddressingMode::ZeroPageX },
		{ 0xb6, 0, 0, 0, Instruction::LDX, AddressingMode::ZeroPageY },
		{ 0xb7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xb8, 0, 0, 0, Instruction::CLV, AddressingMode::Implicit },
		{ 0xb9, 0, 0, 0, Instruction::LDA, AddressingMode::AbsoluteY },
		{ 0xba, 0, 0, 0, Instruction::TSX, AddressingMode::Implicit },
		{ 0xbb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xbc, 0, 0, 0, Instruction::LDY, AddressingMode::AbsoluteX },
		{ 0xbd, 0, 0, 0, Instruction::LDA, AddressingMode::AbsoluteX },
		{ 0xbe, 0, 0, 0, Instruction::LDX, AddressingMode::AbsoluteY },
		{ 0xbf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0xc0, 0, 0, 0, Instruction::CPY, AddressingMode::Immediate },
		{ 0xc1, 0, 0, 0, Instruction::CMP, AddressingMode::XIndexedIndirect },
		{ 0xc2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xc3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xc4, 0, 0, 0, Instruction::CPY, AddressingMode::ZeroPage },
		{ 0xc5, 0, 0, 0, Instruction::CMP, AddressingMode::ZeroPage },
		{ 0xc6, 0, 0, 0, Instruction::DEC, AddressingMode::ZeroPage },
		{ 0xc7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xc8, 0, 0, 0, Instruction::INY, AddressingMode::Implicit },
		{ 0xc9, 0, 0, 0, Instruction::CMP, AddressingMode::Immediate },
		{ 0xca, 0, 0, 0, Instruction::DEX, AddressingMode::Implicit },
		{ 0xcb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xcc, 0, 0, 0, Instruction::CPY, AddressingMode::Absolute },
		{ 0xcd, 0, 0, 0, Instruction::CMP, AddressingMode::Absolute },
		{ 0xce, 0, 0, 0, Instruction::DEC, AddressingMode::Absolute },
		{ 0xcf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0xd0, 0, 0, 0, Instruction::BNE, AddressingMode::Relative },
		{ 0xd1, 0, 0, 0, Instruction::CMP, AddressingMode::IndirectYIndexed },
		{ 0xd2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xd3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xd4, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xd5, 0, 0, 0, Instruction::CMP, AddressingMode::ZeroPageX },
		{ 0xd6, 0, 0, 0, Instruction::DEC, AddressingMode::ZeroPageX },
		{ 0xd7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xd8, 0, 0, 0, Instruction::CLD, AddressingMode::Implicit },
		{ 0xd9, 0, 0, 0, Instruction::CMP, AddressingMode::AbsoluteY },
		{ 0xda, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xdb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xdc, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xdd, 0, 0, 0, Instruction::CMP, AddressingMode::AbsoluteX },
		{ 0xde, 0, 0, 0, Instruction::DEC, AddressingMode::AbsoluteX },
		{ 0xdf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0xe0, 0, 0, 0, Instruction::CPX, AddressingMode::Immediate },
		{ 0xe1, 0, 0, 0, Instruction::SBC, AddressingMode::XIndexedIndirect },
		{ 0xe2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xe3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xe4, 0, 0, 0, Instruction::CPX, AddressingMode::ZeroPage },
		{ 0xe5, 0, 0, 0, Instruction::SBC, AddressingMode::ZeroPage },
		{ 0xe6, 0, 0, 0, Instruction::INC, AddressingMode::ZeroPage },
		{ 0xe7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xe8, 0, 0, 0, Instruction::INX, AddressingMode::Implicit },
		{ 0xe9, 0, 0, 0, Instruction::SBC, AddressingMode::Immediate },
		{ 0xea, 0, 0, 0, Instruction::NOP, AddressingMode::Implicit },
		{ 0xeb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xec, 0, 0, 0, Instruction::CPX, AddressingMode::Absolute },
		{ 0xed, 0, 0, 0, Instruction::SBC, AddressingMode::Absolute },
		{ 0xee, 0, 0, 0, Instruction::INC, AddressingMode::Absolute },
		{ 0xef, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },

		{ 0xf0, 0, 0, 0, Instruction::BEQ, AddressingMode::Relative },
		{ 0xf1, 0, 0, 0, Instruction::SBC, AddressingMode::IndirectYIndexed },
		{ 0xf2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xf3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xf4, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xf5, 0, 0, 0, Instruction::SBC, AddressingMode::ZeroPageX },
		{ 0xf6, 0, 0, 0, Instruction::INC, AddressingMode::ZeroPageX },
		{ 0xf7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xf8, 0, 0, 0, Instruction::SED, AddressingMode::Implicit },
		{ 0xf9, 0, 0, 0, Instruction::SBC, AddressingMode::AbsoluteY },
		{ 0xfa, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xfb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xfc, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
		{ 0xfd, 0, 0, 0, Instruction::SBC, AddressingMode::AbsoluteX },
		{ 0xfe, 0, 0, 0, Instruction::INC, AddressingMode::AbsoluteX },
		{ 0xff, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined },
	};
	


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
	uint8_t stack;


 
	uint16_t programCounter;
}
