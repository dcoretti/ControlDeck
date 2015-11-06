#include "InstructionDispatcher.h"

#include <cstdint>


namespace NES {
    ////////////////////////////////////////////////
    //	Single byte instructions

    void InstructionDispatcher::NOP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        // do nothing
    }

    // Instructions with accumulator addressing mode TODO how to handle...
    void InstructionDispatcher::ASL(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    // Rotate left (0-filled)
    void InstructionDispatcher::ROL(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    // Logical shift right (shift right with 0 bit going to carry flag, 7 bit set to 0.)
    void InstructionDispatcher::LSR(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    // Decrement X register
    void InstructionDispatcher::DEX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x--;
        registers.setFlagIfZero(registers.x);
        registers.setFlagIfNegative(registers.x);
    }

    // Decrement Y register
    void InstructionDispatcher::DEY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y--;
        registers.setFlagIfZero(registers.y);
        registers.setFlagIfNegative(registers.y);
    }

    void InstructionDispatcher::INX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x++;
        registers.setFlagIfZero(registers.x);
        registers.setFlagIfNegative(registers.x);
    }

    void InstructionDispatcher::INY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y++;
        registers.setFlagIfZero(registers.y);
        registers.setFlagIfNegative(registers.y);
    }

    // Transfer accumulator to x
    void TAX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.x = registers.acc;
    }

    // Transfer x to accumulator
    void InstructionDispatcher::TXA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc = registers.x;
    }

    // Transfer y to accumulator
    void InstructionDispatcher::TYA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.acc = registers.y;
    }

    // Transfer accumulator to y
    void InstructionDispatcher::TAY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.y = registers.acc;
    }

    // Clear carry flag
    void InstructionDispatcher::CLC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::CarryFlag);
    }

    // Clear decimal mode flag
    void InstructionDispatcher::CLD(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::DecimalMode);
    }

    // Clear interrupt disable flag
    void InstructionDispatcher::CLI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::InterruptDisable);

    }

    // Clear overflow flag
    void InstructionDispatcher::CLV(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.clearFlag(ProcessorStatus::OverflowFlag);

    }

    // Set carry flag
    void InstructionDispatcher::SEC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.setFlag(ProcessorStatus::CarryFlag);
    }

    // Set decimal flag
    void InstructionDispatcher::SED(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.setFlag(ProcessorStatus::DecimalMode);
    }

    // Set interrupt disable flag
    void InstructionDispatcher::SEI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        registers.setFlag(ProcessorStatus::InterruptDisable);

    }

    /////////////////////////////////////////////////
    // Internally executed instructions
    void InstructionDispatcher::ADC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        
    }

    void InstructionDispatcher::CMP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::EOR(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::LDY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::AND(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::CPX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::LDA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::ORA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::BIT(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::CPY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::LDX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }

    void InstructionDispatcher::SBC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {

    }


    /////////////////////////////////////////////////
    //	Stack instructions

    // Push x to stack pointer
    void InstructionDispatcher::TXS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.x;
        pushRegister(systemBus, registers, memoryMapper);
    }
    // Push accumulator on stack
    void InstructionDispatcher::PHA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.acc;
        pushRegister(systemBus, registers, memoryMapper);
    }
    // Push processor status on stack
    void InstructionDispatcher::PHP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        systemBus.dataBus = registers.statusRegister;
        pushRegister(systemBus, registers, memoryMapper);
    }

    // Pop stack pointer to x
    void InstructionDispatcher::TSX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popRegister(systemBus, registers, memoryMapper);
        registers.x = systemBus.dataBus;
    }
    // Pop accumulator from stack
    void InstructionDispatcher::PLA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popRegister(systemBus, registers, memoryMapper);
        registers.acc = systemBus.dataBus;
    }
    // Pop processor status from stack
    void InstructionDispatcher::PLP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popRegister(systemBus, registers, memoryMapper);
        registers.statusRegister = systemBus.dataBus;
    }


    // Stack is pushed to from X register to memory location $0100 + Stack pointer offset (00-ff)
    // No overflow detection just like the NES
    void InstructionDispatcher::pushStackSetup(SystemBus &systemBus, Registers &registers) {
        systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
        registers.stackPointer += 8;
        systemBus.read = false;
    }

    // Data must be transfered to register X after read on stack instruction.  
    // TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
    void InstructionDispatcher::popStackSetup(SystemBus &systemBus, Registers &registers) {
        systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
        registers.stackPointer -= 8;
        systemBus.read = true;
    }

    void InstructionDispatcher::popRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);

        // Update processor status flags
        if (systemBus.dataBus == 0) {
            registers.flagSet(ProcessorStatus::ZeroFlag);
        }
        registers.setFlagIfNegative(systemBus.dataBus);
    }

    void InstructionDispatcher::pushRegister(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        pushStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
    }
}