#include "InstructionDispatcher.h"
#include <cstdint>


namespace NES {
   const OpCode InstructionDispatcher::opCodes[256] = {
        { 0x00, 1, 7, 0, Instruction::BRK, AddressingMode::Implicit, &BRK },
        { 0x01, 2, 6, 0, Instruction::ORA, AddressingMode::XIndexedIndirect, &ORA },
        { 0x02, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x03, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x04, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x05, 2, 3, 0, Instruction::ORA, AddressingMode::ZeroPage, &ORA },
        { 0x06, 2, 5, 0, Instruction::ASL, AddressingMode::ZeroPage, &ASL },
        { 0x07, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x08, 1, 3, 0, Instruction::PHP, AddressingMode::Implicit, &PHP },
        { 0x09, 2, 2, 0, Instruction::ORA, AddressingMode::Immediate, &ORA },
        { 0x0a, 1, 2, 0, Instruction::ASL, AddressingMode::Accumulator, &ASL },
        { 0x0b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x0c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x0d, 3, 4, 0, Instruction::ORA, AddressingMode::Absolute, &ORA },
        { 0x0e, 3, 6, 0, Instruction::ASL, AddressingMode::Absolute, &ASL },
        { 0x0f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x10, 2, 2, 2, Instruction::BPL, AddressingMode::Relative, &BPL },
        { 0x11, 2, 5, 1, Instruction::ORA, AddressingMode::IndirectYIndexed, &ORA },
        { 0x12, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x13, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x14, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x15, 2, 4, 0, Instruction::ORA, AddressingMode::ZeroPageX, &ORA },
        { 0x16, 2, 6, 0, Instruction::ASL, AddressingMode::ZeroPageX, &ASL },
        { 0x17, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x18, 1, 2, 0, Instruction::CLC, AddressingMode::Implicit, &CLC },
        { 0x19, 3, 4, 1, Instruction::ORA, AddressingMode::AbsoluteY, &ORA },
        { 0x1a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x1b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x1c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x1d, 3, 4, 1, Instruction::ORA, AddressingMode::AbsoluteX, &ORA },
        { 0x1e, 3, 7, 0, Instruction::ASL, AddressingMode::AbsoluteX, &ASL },
        { 0x1f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x20, 3, 6, 0, Instruction::JSR, AddressingMode::Absolute, &JSR },
        { 0x21, 2, 6, 0, Instruction::AND, AddressingMode::XIndexedIndirect, &AND },
        { 0x22, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x23, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x24, 2, 3, 0, Instruction::BIT, AddressingMode::ZeroPage, &BIT },
        { 0x25, 2, 3, 0, Instruction::AND, AddressingMode::ZeroPage, &AND },
        { 0x26, 2, 5, 0, Instruction::ROL, AddressingMode::ZeroPage, &ROL },
        { 0x27, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x28, 1, 4, 0, Instruction::PLP, AddressingMode::Implicit, &PLP },
        { 0x29, 2, 2, 0, Instruction::AND, AddressingMode::Immediate, &AND },
        { 0x2a, 1, 2, 0, Instruction::ROL, AddressingMode::Accumulator, &ROL },
        { 0x2b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x2c, 3, 4, 0, Instruction::BIT, AddressingMode::Absolute, &BIT },
        { 0x2d, 3, 4, 0, Instruction::AND, AddressingMode::Absolute, &AND },
        { 0x2e, 3, 6, 0, Instruction::ROL, AddressingMode::Absolute, &ROL },
        { 0x2f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x30, 2, 2, 2, Instruction::BMI, AddressingMode::Relative, &BMI },
        { 0x31, 2, 5, 1, Instruction::AND, AddressingMode::IndirectYIndexed, &AND },
        { 0x32, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x33, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x34, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x35, 2, 4, 0, Instruction::AND, AddressingMode::ZeroPageX, &AND },
        { 0x36, 2, 6, 0, Instruction::ROL, AddressingMode::ZeroPageX, &ROL },
        { 0x37, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x38, 1, 2, 0, Instruction::SEC, AddressingMode::Implicit, &SEC },
        { 0x39, 3, 4, 1, Instruction::AND, AddressingMode::AbsoluteY, &AND },
        { 0x3a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x3b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x3c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x3d, 3, 4, 1, Instruction::AND, AddressingMode::AbsoluteX, &AND },
        { 0x3e, 3, 7, 0, Instruction::ROL, AddressingMode::AbsoluteX, &ROL },
        { 0x3f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x40, 1, 6, 0, Instruction::RTI, AddressingMode::Implicit, &RTI },
        { 0x41, 2, 6, 0, Instruction::EOR, AddressingMode::XIndexedIndirect, &EOR },
        { 0x42, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x43, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x44, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x45, 2, 3, 0, Instruction::EOR, AddressingMode::ZeroPage, &EOR },
        { 0x46, 2, 5, 0, Instruction::LSR, AddressingMode::ZeroPage, &LSR },
        { 0x47, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x48, 1, 3, 0, Instruction::PHA, AddressingMode::Implicit, &PHA },
        { 0x49, 2, 2, 0, Instruction::EOR, AddressingMode::Immediate, &EOR },
        { 0x4a, 1, 2, 0, Instruction::LSR, AddressingMode::Accumulator, &LSR },
        { 0x4b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x4c, 3, 3, 0, Instruction::JMP, AddressingMode::Absolute, &JMP },
        { 0x4d, 3, 4, 0, Instruction::EOR, AddressingMode::Absolute, &EOR },
        { 0x4e, 3, 6, 0, Instruction::LSR, AddressingMode::Absolute, &LSR },
        { 0x4f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x50, 2, 2, 2, Instruction::BVC, AddressingMode::Relative, &BVC },
        { 0x51, 2, 5, 1, Instruction::EOR, AddressingMode::IndirectYIndexed, &EOR },
        { 0x52, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x53, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x54, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x55, 2, 4, 0, Instruction::EOR, AddressingMode::ZeroPageX, &EOR },
        { 0x56, 2, 6, 0, Instruction::LSR, AddressingMode::ZeroPageX, &LSR },
        { 0x57, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x58, 1, 2, 0, Instruction::CLI, AddressingMode::Implicit, &CLI },
        { 0x59, 3, 4, 1, Instruction::EOR, AddressingMode::AbsoluteY, &EOR },
        { 0x5a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x5b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x5c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x5d, 3, 4, 1, Instruction::EOR, AddressingMode::AbsoluteX, &EOR },
        { 0x5e, 3, 7, 0, Instruction::LSR, AddressingMode::AbsoluteX, &LSR },
        { 0x5f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x60, 1, 6, 0, Instruction::RTS, AddressingMode::Implicit, &RTS },
        { 0x61, 2, 6, 0, Instruction::ADC, AddressingMode::Indirect, &ADC },
        { 0x62, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x63, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x64, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x65, 2, 3, 0, Instruction::ADC, AddressingMode::ZeroPage, &ADC },
        { 0x66, 2, 5, 0, Instruction::ROR, AddressingMode::ZeroPage, &ROR },
        { 0x67, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x68, 1, 4, 0, Instruction::PLA, AddressingMode::Implicit, &PLA },
        { 0x69, 2, 2, 0, Instruction::ADC, AddressingMode::Immediate, &ADC },
        { 0x6a, 1, 2, 0, Instruction::ROR, AddressingMode::Accumulator, &ROR },
        { 0x6b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x6c, 3, 3, 0, Instruction::JMP, AddressingMode::Indirect, &JMP },
        { 0x6d, 3, 4, 0, Instruction::ADC, AddressingMode::Absolute, &ADC },
        { 0x6e, 3, 6, 0, Instruction::ROR, AddressingMode::Absolute, &ROR },
        { 0x6f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x70, 2, 2, 2, Instruction::BVS, AddressingMode::Relative, &BVS },
        { 0x71, 2, 5, 1, Instruction::ADC, AddressingMode::IndirectYIndexed, &ADC },
        { 0x72, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x73, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x74, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x75, 2, 4, 0, Instruction::ADC, AddressingMode::ZeroPageX, &ADC },
        { 0x76, 2, 6, 0, Instruction::ROR, AddressingMode::ZeroPageX, &ROR },
        { 0x77, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x78, 1, 2, 0, Instruction::SEI, AddressingMode::Implicit, &SEI },
        { 0x79, 3, 4, 1, Instruction::ADC, AddressingMode::AbsoluteY, &ADC },
        { 0x7a, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x7b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x7c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x7d, 3, 4, 1, Instruction::ADC, AddressingMode::AbsoluteX, &ADC },
        { 0x7e, 3, 7, 0, Instruction::ROR, AddressingMode::AbsoluteX, &ROR },
        { 0x7f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x80, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x81, 2, 6, 0, Instruction::STA, AddressingMode::Indirect, &STA },
        { 0x82, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x83, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x84, 2, 3, 0, Instruction::STY, AddressingMode::ZeroPage, &STY },
        { 0x85, 2, 3, 0, Instruction::STA, AddressingMode::ZeroPage, &STA },
        { 0x86, 2, 3, 0, Instruction::STX, AddressingMode::ZeroPage, &STX },
        { 0x87, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x88, 1, 2, 0, Instruction::DEY, AddressingMode::Implicit, &DEY },
        { 0x89, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x8a, 1, 2, 0, Instruction::TXA, AddressingMode::Implicit, &TXA },
        { 0x8b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x8c, 3, 4, 0, Instruction::STY, AddressingMode::Absolute, &STY },
        { 0x8d, 3, 4, 0, Instruction::STA, AddressingMode::Absolute, &STA },
        { 0x8e, 3, 4, 0, Instruction::STX, AddressingMode::Absolute, &STX },
        { 0x8f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0x90, 2, 2, 2, Instruction::BCC, AddressingMode::Relative, &BCC },
        { 0x91, 2, 6, 0, Instruction::STA, AddressingMode::IndirectYIndexed, &STA },
        { 0x92, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x93, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x94, 2, 4, 0, Instruction::STY, AddressingMode::ZeroPageX, &STY },
        { 0x95, 2, 4, 0, Instruction::STA, AddressingMode::ZeroPageX, &STA },
        { 0x96, 2, 4, 0, Instruction::STX, AddressingMode::ZeroPageY, &STX },
        { 0x97, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x98, 1, 2, 0, Instruction::TYA, AddressingMode::Implicit, &TYA },
        { 0x99, 3, 5, 0, Instruction::STA, AddressingMode::AbsoluteY, &STA },
        { 0x9a, 1, 2, 0, Instruction::TXS, AddressingMode::Implicit, &TXS },
        { 0x9b, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x9c, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x9d, 3, 5, 0, Instruction::STA, AddressingMode::AbsoluteX, &STA },
        { 0x9e, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0x9f, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0xa0, 2, 2, 0, Instruction::LDY, AddressingMode::Immediate, &LDY },
        { 0xa1, 2, 6, 0, Instruction::LDA, AddressingMode::XIndexedIndirect, &LDA },
        { 0xa2, 2, 2, 0, Instruction::LDX, AddressingMode::Immediate, &LDX },
        { 0xa3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xa4, 2, 3, 0, Instruction::LDY, AddressingMode::ZeroPage, &LDY },
        { 0xa5, 2, 3, 0, Instruction::LDA, AddressingMode::ZeroPage, &LDA },
        { 0xa6, 2, 3, 0, Instruction::LDX, AddressingMode::ZeroPage, &LDX },
        { 0xa7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xa8, 1, 2, 0, Instruction::TAY, AddressingMode::Implicit, &TAY },
        { 0xa9, 2, 2, 0, Instruction::LDA, AddressingMode::Immediate, &LDA },
        { 0xaa, 1, 2, 0, Instruction::TAX, AddressingMode::Implicit, &TAX },
        { 0xab, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xac, 0, 0, 0, Instruction::LDY, AddressingMode::Absolute, &LDY },
        { 0xad, 3, 4, 0, Instruction::LDA, AddressingMode::Absolute, &LDA },
        { 0xae, 3, 4, 0, Instruction::LDX, AddressingMode::Absolute, &LDX },
        { 0xaf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0xb0, 2, 2, 2, Instruction::BCS, AddressingMode::Relative, &BCS },
        { 0xb1, 2, 5, 1, Instruction::LDA, AddressingMode::IndirectYIndexed, &LDA },
        { 0xb2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xb3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xb4, 2, 4, 0, Instruction::LDY, AddressingMode::ZeroPageX, &LDY },
        { 0xb5, 2, 4, 0, Instruction::LDA, AddressingMode::ZeroPageX, &LDA },
        { 0xb6, 2, 4, 0, Instruction::LDX, AddressingMode::ZeroPageY, &LDX },
        { 0xb7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xb8, 1, 2, 0, Instruction::CLV, AddressingMode::Implicit, &CLV },
        { 0xb9, 3, 4, 1, Instruction::LDA, AddressingMode::AbsoluteY, &LDA },
        { 0xba, 1, 2, 0, Instruction::TSX, AddressingMode::Implicit, &TSX },
        { 0xbb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xbc, 3, 4, 1, Instruction::LDY, AddressingMode::AbsoluteX, &LDY },
        { 0xbd, 3, 4, 1, Instruction::LDA, AddressingMode::AbsoluteX, &LDA },
        { 0xbe, 3, 4, 1, Instruction::LDX, AddressingMode::AbsoluteY, &LDX },
        { 0xbf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0xc0, 2, 2, 0, Instruction::CPY, AddressingMode::Immediate, &CPY },
        { 0xc1, 2, 6, 0, Instruction::CMP, AddressingMode::XIndexedIndirect, &CMP },
        { 0xc2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xc3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xc4, 2, 3, 0, Instruction::CPY, AddressingMode::ZeroPage, &CPY },
        { 0xc5, 2, 3, 0, Instruction::CMP, AddressingMode::ZeroPage, &CMP },
        { 0xc6, 2, 5, 0, Instruction::DEC, AddressingMode::ZeroPage, &DEC },
        { 0xc7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xc8, 1, 2, 0, Instruction::INY, AddressingMode::Implicit, &INY },
        { 0xc9, 2, 2, 0, Instruction::CMP, AddressingMode::Immediate, &CMP },
        { 0xca, 1, 2, 0, Instruction::DEX, AddressingMode::Implicit, &DEX },
        { 0xcb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xcc, 2, 4, 0, Instruction::CPY, AddressingMode::Absolute, &CPY },
        { 0xcd, 3, 4, 0, Instruction::CMP, AddressingMode::Absolute, &CMP },
        { 0xce, 3, 3, 0, Instruction::DEC, AddressingMode::Absolute, &DEC },
        { 0xcf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0xd0, 2, 2, 2, Instruction::BNE, AddressingMode::Relative, &BNE },
        { 0xd1, 2, 5, 1, Instruction::CMP, AddressingMode::IndirectYIndexed, &CMP },
        { 0xd2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xd3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xd4, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xd5, 2, 4, 0, Instruction::CMP, AddressingMode::ZeroPageX, &CMP },
        { 0xd6, 2, 6, 0, Instruction::DEC, AddressingMode::ZeroPageX, &DEC },
        { 0xd7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xd8, 1, 2, 0, Instruction::CLD, AddressingMode::Implicit, &CLD },
        { 0xd9, 3, 4, 1, Instruction::CMP, AddressingMode::AbsoluteY, &CMP },
        { 0xda, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xdb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xdc, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xdd, 3, 4, 1, Instruction::CMP, AddressingMode::AbsoluteX, &CMP },
        { 0xde, 3, 7, 0, Instruction::DEC, AddressingMode::AbsoluteX, &DEC },
        { 0xdf, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0xe0, 2, 2, 0, Instruction::CPX, AddressingMode::Immediate, &CPX },
        { 0xe1, 2, 6, 0, Instruction::SBC, AddressingMode::XIndexedIndirect, &SBC },
        { 0xe2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xe3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xe4, 2, 3, 0, Instruction::CPX, AddressingMode::ZeroPage, &CPX },
        { 0xe5, 2, 3, 0, Instruction::SBC, AddressingMode::ZeroPage, &SBC },
        { 0xe6, 2, 5, 0, Instruction::INC, AddressingMode::ZeroPage, &INC },
        { 0xe7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xe8, 1, 2, 0, Instruction::INX, AddressingMode::Implicit, &INX },
        { 0xe9, 2, 2, 0, Instruction::SBC, AddressingMode::Immediate, &SBC },
        { 0xea, 1, 2, 0, Instruction::NOP, AddressingMode::Implicit, &NOP },
        { 0xeb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xec, 3, 4, 0, Instruction::CPX, AddressingMode::Absolute, &CPX },
        { 0xed, 3, 4, 0, Instruction::SBC, AddressingMode::Absolute, &SBC },
        { 0xee, 3, 6, 0, Instruction::INC, AddressingMode::Absolute, &INC },
        { 0xef, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },

        { 0xf0, 2, 2, 2, Instruction::BEQ, AddressingMode::Relative, &BEQ },
        { 0xf1, 2, 5, 1, Instruction::SBC, AddressingMode::IndirectYIndexed, &SBC },
        { 0xf2, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xf3, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xf4, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xf5, 2, 4, 0, Instruction::SBC, AddressingMode::ZeroPageX, &SBC },
        { 0xf6, 2, 6, 0, Instruction::INC, AddressingMode::ZeroPageX, &INC },
        { 0xf7, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xf8, 1, 2, 0, Instruction::SED, AddressingMode::Implicit, &SED },
        { 0xf9, 3, 4, 1, Instruction::SBC, AddressingMode::AbsoluteY, &SBC },
        { 0xfa, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xfb, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xfc, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK },
        { 0xfd, 3, 4, 1, Instruction::SBC, AddressingMode::AbsoluteX, &SBC },
        { 0xfe, 3, 7, 0, Instruction::INC, AddressingMode::AbsoluteX, &INC },
        { 0xff, 0, 0, 0, Instruction::UNK, AddressingMode::Undefined, &UNK }
    };
    InstructionDispatcher::InstructionDispatcher(): addressingModeHandler() {
    }
    ////////////////////////////////////////////////
    //	Single byte instructions

    void InstructionDispatcher::dispatchInstruction(uint8_t operation, SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        OpCode opCode = opCodes[operation];
        // Set up system bus to contain relevant memory data for a particular instruction.
        addressingModeHandler.handleAddressingMode(opCode.addressingMode, systemBus, registers, memoryMapper);

        // Call the instruction handler
        opCode.instructionHandler(opCode, systemBus, registers, memoryMapper);
    }

    void InstructionDispatcher::NOP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        // do nothing
    }

    // signed shift
    void InstructionDispatcher::ASL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t arg = systemBus.dataBus;
        if (opCode.addressingMode == AddressingMode::Accumulator) {
            arg = registers.acc;
        }

        if ((arg & 0x80) != 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }
        arg = (int8_t)arg << 1;

        if (opCode.addressingMode == AddressingMode::Accumulator) {
            registers.acc = arg;
        } else {
            // write back to memory location
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }
    }

    // Rotate left (0-filled)
    void InstructionDispatcher::ROL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t arg = systemBus.dataBus;
        if (opCode.addressingMode == AddressingMode::Accumulator) {
            arg = registers.acc;
        }

        bool carrySet = registers.flagSet(ProcessorStatus::CarryFlag);
        if ((arg & 0x80) != 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }
        arg = arg << (uint8_t)1;
        arg += carrySet ? 1 : 0;

        registers.setFlagIfZero(arg);
        registers.setFlagIfNegative(arg);

        if (opCode.addressingMode == AddressingMode::Accumulator) {
            registers.acc = arg;
        }
        else {
            // write back to memory location
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }
    }

    void InstructionDispatcher::ROR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t arg = systemBus.dataBus;
        if (opCode.addressingMode == AddressingMode::Accumulator) {
            arg = registers.acc;
        }
        uint8_t carryMask = registers.flagSet(ProcessorStatus::CarryFlag) ? 0x80 : 0x00;
        if ((arg & 0x01) != 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }
        arg = arg >> (uint8_t)1;
        // old carry flag is brought in as bit 7;
        arg &= carryMask;

        registers.setFlagIfZero(arg);
        registers.setFlagIfNegative(arg);

        if (opCode.addressingMode == AddressingMode::Accumulator) {
            registers.acc = arg;
        }
        else {
            // write back to memory location
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }
    }

    // Logical shift right (shift right with 0 bit going to carry flag, 7 bit set to 0.)
    void InstructionDispatcher::LSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t arg = systemBus.dataBus;
        if (opCode.addressingMode == AddressingMode::Accumulator) {
            arg = registers.acc;
        }

        if ((arg & 0x01) != 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }
        arg = arg >> (uint8_t)1;
        registers.setFlagIfZero(arg);
        registers.setFlagIfNegative(arg);

        if (opCode.addressingMode == AddressingMode::Accumulator) {
            registers.acc = arg;
        }
        else {
            // write back to memory location
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }
    }

    void InstructionDispatcher::DEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus--;
        registers.setFlagIfZero(systemBus.dataBus);
        registers.setFlagIfNegative(systemBus.dataBus);

        systemBus.read = false;
        memoryMapper.doMemoryOperation(systemBus);
    }

    void InstructionDispatcher::INC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus++;
        registers.setFlagIfZero(systemBus.dataBus);
        registers.setFlagIfNegative(systemBus.dataBus);

        systemBus.read = false;
        memoryMapper.doMemoryOperation(systemBus);
    }

    // Decrement X register
    void InstructionDispatcher::DEX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x--;
        registers.setFlagIfZero(registers.x);
        registers.setFlagIfNegative(registers.x);
    }

    // Decrement Y register
    void InstructionDispatcher::DEY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y--;
        registers.setFlagIfZero(registers.y);
        registers.setFlagIfNegative(registers.y);
    }

    void InstructionDispatcher::INX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x++;
        registers.setFlagIfZero(registers.x);
        registers.setFlagIfNegative(registers.x);
    }

    void InstructionDispatcher::INY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y++;
        registers.setFlagIfZero(registers.y);
        registers.setFlagIfNegative(registers.y);
    }

    // Transfer accumulator to x
    void InstructionDispatcher::TAX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x = registers.acc;
    }

    // Transfer x to accumulator
    void InstructionDispatcher::TXA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc = registers.x;
    }

    // Transfer y to accumulator
    void InstructionDispatcher::TYA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc = registers.y;
    }

    // Transfer accumulator to y
    void InstructionDispatcher::TAY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y = registers.acc;
    }

    // Clear carry flag
    void InstructionDispatcher::CLC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::CarryFlag);
    }

    // Clear decimal mode flag
    void InstructionDispatcher::CLD(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::DecimalMode);
    }

    // Clear interrupt disable flag
    void InstructionDispatcher::CLI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::InterruptDisable);

    }

    // Clear overflow flag
    void InstructionDispatcher::CLV(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::OverflowFlag);

    }

    // Set carry flag
    void InstructionDispatcher::SEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.setFlag(ProcessorStatus::CarryFlag);
    }

    // Set decimal flag
    void InstructionDispatcher::SED(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.setFlag(ProcessorStatus::DecimalMode);
    }

    // Set interrupt disable flag
    void InstructionDispatcher::SEI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.setFlag(ProcessorStatus::InterruptDisable);

    }

    // TODO .. simplify these
    void InstructionDispatcher::ADC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t add = registers.acc + systemBus.dataBus;
        if (registers.willAddOverflow(systemBus.dataBus)) {
            registers.setFlag(ProcessorStatus::OverflowFlag);
        }
        registers.setFlagIfZero(add);
        if (registers.willAddCarry(systemBus.dataBus)) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }
        registers.setFlagIfNegative(add);

        registers.acc = add;
    }

    void InstructionDispatcher::SBC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t sub = registers.acc - systemBus.dataBus;
        if (registers.willSubOverflow(systemBus.dataBus)) {
            registers.setFlag(ProcessorStatus::OverflowFlag);
        }
        registers.setFlagIfZero(sub);
        if (registers.willSubCarry(systemBus.dataBus)) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }
        registers.setFlagIfNegative(sub);

        registers.acc = sub;
    }

    /////////////////////////////////////////////////
    // Logical operations on accumulator
    void InstructionDispatcher::EOR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc ^= systemBus.dataBus;
        registers.setFlagIfZero(registers.acc);
        registers.setFlagIfNegative(registers.acc);
    }

    void InstructionDispatcher::AND(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc &= systemBus.dataBus;
        registers.setFlagIfZero(registers.acc);
        registers.setFlagIfNegative(registers.acc);
    }

    void InstructionDispatcher::ORA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc |= systemBus.dataBus;
        registers.setFlagIfZero(registers.acc);
        registers.setFlagIfNegative(registers.acc);
    }

    void InstructionDispatcher::LDA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc = systemBus.dataBus;
        registers.setFlagIfZero(registers.acc);
        registers.setFlagIfNegative(registers.acc);
    }

    void InstructionDispatcher::LDY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y = systemBus.dataBus;
        registers.setFlagIfZero(registers.y);
        registers.setFlagIfNegative(registers.y);
    }

    void InstructionDispatcher::LDX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x = systemBus.dataBus;
        registers.setFlagIfZero(registers.x);
        registers.setFlagIfNegative(registers.x);
    }

    // Bit test by & with acc
    void InstructionDispatcher::BIT(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        uint8_t val = registers.acc & systemBus.dataBus;
        registers.setFlagIfZero(val);
        registers.setFlagIfNegative(systemBus.dataBus);
        
        if ((systemBus.dataBus & (1 << 6)) != 0) {
            registers.setFlag(ProcessorStatus::OverflowFlag);
        }
    }

    void InstructionDispatcher::CMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        int8_t sub = (int8_t)registers.acc - (int8_t)systemBus.dataBus;
        if (sub == 0) {
            registers.setFlag(ProcessorStatus::ZeroFlag);
        }
        if (sub >= 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }

        registers.setFlagIfNegative((uint8_t)sub);
    }

    void InstructionDispatcher::CPY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        int8_t sub = (int8_t)registers.y - (int8_t)systemBus.dataBus;
        if (sub == 0) {
            registers.setFlag(ProcessorStatus::ZeroFlag);
        }
        if (sub >= 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }

        registers.setFlagIfNegative((uint8_t)sub);
    }


    void InstructionDispatcher::CPX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        int8_t sub = (int8_t)registers.x - (int8_t)systemBus.dataBus;
        if (sub == 0) {
            registers.setFlag(ProcessorStatus::ZeroFlag);
        } 
        if (sub >= 0) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }

        registers.setFlagIfNegative((uint8_t)sub);
    }



    /////////////////////////////////////////////////
    //	Stack instructions

    // Push x to stack pointer
    void InstructionDispatcher::TXS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.x;
        pushDataBusToStack(systemBus, registers, memoryMapper);
    }
    // Push accumulator on stack
    void InstructionDispatcher::PHA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.acc;
        pushDataBusToStack(systemBus, registers, memoryMapper);
    }
    // Push processor status on stack
    void InstructionDispatcher::PHP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.statusRegister;
        pushDataBusToStack(systemBus, registers, memoryMapper);
    }

    // Pop stack pointer to x
    void InstructionDispatcher::TSX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackToDataBus(systemBus, registers, memoryMapper);
        registers.x = systemBus.dataBus;
    }
    // Pop accumulator from stack
    void InstructionDispatcher::PLA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackToDataBus(systemBus, registers, memoryMapper);
        registers.acc = systemBus.dataBus;
    }
    // Pop processor status from stack
    void InstructionDispatcher::PLP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackToDataBus(systemBus, registers, memoryMapper);
        registers.statusRegister = systemBus.dataBus;
    }

    void InstructionDispatcher::BCC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (!registers.flagSet(ProcessorStatus::CarryFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BCS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (registers.flagSet(ProcessorStatus::CarryFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BEQ(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (registers.flagSet(ProcessorStatus::ZeroFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BPL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (!registers.flagSet(ProcessorStatus::ZeroFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BMI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (registers.flagSet(ProcessorStatus::NegativeFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BNE(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (!registers.flagSet(ProcessorStatus::NegativeFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BVC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (!registers.flagSet(ProcessorStatus::OverflowFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::BVS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (registers.flagSet(ProcessorStatus::OverflowFlag)) {
            registers.programCounter += systemBus.dataBus;
        }
    }

    void InstructionDispatcher::JMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.programCounter = systemBus.addressBus;
    }

    void InstructionDispatcher::BRK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        // push program counter
        systemBus.dataBus = registers.pch();
        pushDataBusToStack(systemBus, registers, memoryMapper);
        systemBus.dataBus = registers.pcl();
        pushDataBusToStack(systemBus, registers, memoryMapper);
        // push status register
        systemBus.dataBus = registers.statusRegister;
        pushDataBusToStack(systemBus, registers, memoryMapper);

        // fetch interrupt vector from 0xfffe/f
        systemBus.addressBus = (uint16_t)0xffffe;
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
        uint8_t adl = systemBus.dataBus;

        systemBus.addressBus = (uint16_t)0xfffff;
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
        registers.programCounter = (systemBus.dataBus << 8) + adl;

        registers.setFlag(ProcessorStatus::BreakCommand);
    }


    void InstructionDispatcher::RTI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        // pop stack to processor flags
        popStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
        registers.statusRegister = systemBus.dataBus;
        // pop program counter
        popStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
        registers.setPcl(systemBus.dataBus);
        popStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
        registers.setPch(systemBus.dataBus);
    }


    void InstructionDispatcher::JSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.programCounter = systemBus.addressBus;
        // Usual timing order only fetches ADL here
        systemBus.dataBus = registers.pch();
        pushDataBusToStack(systemBus, registers, memoryMapper);
        systemBus.dataBus = registers.pcl();
        pushDataBusToStack(systemBus, registers, memoryMapper);
        // Usual timing order only fetches ADH here
    }

    void InstructionDispatcher::RTS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackToDataBus(systemBus, registers, memoryMapper);
        registers.setPcl(systemBus.dataBus);
        popStackToDataBus(systemBus, registers, memoryMapper);
        registers.setPch(systemBus.dataBus);

        // PC needs to be incremented by 1 to get next isntruction
    }

    void InstructionDispatcher::STA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.acc;
        systemBus.read = false;
        memoryMapper.doMemoryOperation(systemBus);
    }

    void InstructionDispatcher::STX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.x;
        systemBus.read = false;
        memoryMapper.doMemoryOperation(systemBus);
    }

    void InstructionDispatcher::STY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.y;
        systemBus.read = false;
        memoryMapper.doMemoryOperation(systemBus);
    }


    void InstructionDispatcher::UNK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        // TODO do something to stop execution if we get here?
    }


    //////////////////////////////////////////////////////
    // Utility methods
    // Stack is pushed to from register to memory location $0100 + Stack pointer offset (00-ff)
    // No overflow detection just like the NES

    void InstructionDispatcher::pushStackSetup(SystemBus &systemBus, Registers &registers) {
        systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
        registers.stackPointer += 8;
        systemBus.read = false;
    }

    // TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
    void InstructionDispatcher::popStackSetup(SystemBus &systemBus, Registers &registers) {
        systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
        registers.stackPointer -= 8;
        systemBus.read = true;
    }

    void InstructionDispatcher::popStackToDataBus(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
    }

    void InstructionDispatcher::popStackToDataBusWithFlags(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackToDataBus(systemBus, registers, memoryMapper);

        // Update processor status flags
        if (systemBus.dataBus == 0) {
            registers.flagSet(ProcessorStatus::ZeroFlag);
        }
        registers.setFlagIfNegative(systemBus.dataBus);
    }

    void InstructionDispatcher::pushDataBusToStack(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        pushStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
    }
}