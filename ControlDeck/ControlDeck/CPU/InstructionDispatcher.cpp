#include "InstructionDispatcher.h"
#include <cstdint>


namespace NES {
    InstructionDispatcher::InstructionDispatcher(): addressingModeHandler() {
    }
    ////////////////////////////////////////////////
    //	Single byte instructions

    void InstructionDispatcher::dispatchInstruction(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        // Set up system bus to contain relevant memory data for a particular instruction.
        addressingModeHandler.handleAddressingMode(opCode.addressingMode, systemBus, registers, memoryMapper);

        // Call the instruction handler

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