#include "InstructionDispatcher.h"
#include <cstdint>
#include "../common.h"

namespace NES {
    const uint16_t stackBaseAddress = 0x100;

    namespace InstructionSet {


        uint8_t NOP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            // do nothing
            return 0;
        }

        // signed shift
        uint8_t ASL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            uint8_t arg = systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = registers.acc;
            }

            if ((arg & 0x80) != 0) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                registers.clearFlag(ProcessorStatus::CarryFlag);
            }
            arg = (int8_t)arg << 1;
            registers.setFlagIfZero(arg);
            registers.setFlagIfNegative(arg);
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                registers.acc = arg;
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }

            return 0;
        }

        // Rotate left (0-filled)
        uint8_t ROL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            uint8_t arg = systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = registers.acc;
            }

            bool carrySet = registers.flagSet(ProcessorStatus::CarryFlag);
            if ((arg & 0x80) != 0) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                registers.clearFlag(ProcessorStatus::CarryFlag);
            }
            arg = arg << (uint8_t)1;
            arg += carrySet ? 1 : 0;

            registers.setFlagIfZero(arg);
            registers.setFlagIfNegative(arg);

            if (opCode.addressingMode == AddressingMode::Accumulator) {
                registers.acc = arg;
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }

            return 0;
        }

        uint8_t ROR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            uint8_t arg = systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = registers.acc;
            }
            uint8_t carryMask = registers.flagSet(ProcessorStatus::CarryFlag) ? 0x80 : 0x00;
            if ((arg & 0x01) != 0) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                registers.clearFlag(ProcessorStatus::CarryFlag);
            }

            arg = arg >> (uint8_t)1;
            // old carry flag is brought in as bit 7;
            arg &= carryMask;

            registers.setFlagIfZero(arg);
            registers.setFlagIfNegative(arg);

            if (opCode.addressingMode == AddressingMode::Accumulator) {
                registers.acc = arg;
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }
            return 0;
        }

        // Logical shift right (shift right with 0 bit going to carry flag, 7 bit set to 0.)
        uint8_t LSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            uint8_t arg = systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = registers.acc;
            }

            if ((arg & 0x1) != 0) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                registers.clearFlag(ProcessorStatus::CarryFlag);
            }
            arg = arg >> (uint8_t)1;
            registers.setFlagIfZero(arg);
            registers.setFlagIfNegative(arg);

            if (opCode.addressingMode == AddressingMode::Accumulator) {
                registers.acc = arg;
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }
            return 0;
        }

        uint8_t DEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus--;
            registers.setFlagIfZero(systemBus.dataBus);
            registers.setFlagIfNegative(systemBus.dataBus);

            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);

            return 0;
        }

        uint8_t INC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus++;
            registers.setFlagIfZero(systemBus.dataBus);
            registers.setFlagIfNegative(systemBus.dataBus);

            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);

            return 0;
        }

        // Decrement X register
        uint8_t DEX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x--;
            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);

            return 0;
        }

        // Decrement Y register
        uint8_t DEY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y--;
            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);

            return 0;
        }

        uint8_t INX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x++;
            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);

            return 0;
        }

        uint8_t INY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y++;
            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);

            return 0;
        }

        // Transfer accumulator to x
        uint8_t TAX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x = registers.acc;

            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);

            return 0;
        }

        // Transfer x to accumulator
        uint8_t TXA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = registers.x;

            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);

            return 0;
        }

        // Transfer y to accumulator
        uint8_t TYA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = registers.y;

            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);

            return 0;
        }

        // Transfer accumulator to y
        uint8_t TAY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y = registers.acc;

            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);

            return 0;
        }

        // Clear carry flag
        uint8_t CLC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::CarryFlag);

            return 0;
        }

        // Clear decimal mode flag
        uint8_t CLD(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::DecimalMode);

            return 0;
        }

        // Clear interrupt disable flag
        uint8_t CLI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::InterruptDisable);

            return 0;
        }

        // Clear overflow flag
        uint8_t CLV(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::OverflowFlag);

            return 0;
        }

        // Set carry flag
        uint8_t SEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.setFlag(ProcessorStatus::CarryFlag);

            return 0;
        }

        // Set decimal flag
        uint8_t SED(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.setFlag(ProcessorStatus::DecimalMode);

            return 0;
        }

        // Set interrupt disable flag
        uint8_t SEI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.setFlag(ProcessorStatus::InterruptDisable);

            return 0;
        }

        uint8_t add(uint8_t busVal, Registers &registers) {
            uint16_t add = registers.acc + busVal + registers.flagSet(ProcessorStatus::CarryFlag);
            if (add > 0xff) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            }
            if (registers.willAddOverflow(busVal)) {
                registers.setFlag(ProcessorStatus::OverflowFlag);
            }
            registers.setFlagIfZero((uint8_t)add);
            registers.setFlagIfNegative((uint8_t)add);
            return (uint8_t)add;
        }

        // TODO .. simplify these
        uint8_t ADC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = add(systemBus.dataBus, registers);
            return 0;
        }

        uint8_t SBC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = add(~systemBus.dataBus, registers);
            return 0;
        }

        /////////////////////////////////////////////////
        // Logical operations on accumulator
        uint8_t EOR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc ^= systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
            return 0;
        }

        uint8_t AND(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc &= systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
            return 0;
        }

        uint8_t ORA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc |= systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
            return 0;
        }

        uint8_t LDA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
            return 0;
        }

        uint8_t LDY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y = systemBus.dataBus;
            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);
            return 0;
        }

        uint8_t LDX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x = systemBus.dataBus;
            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);
            return 0;
        }

        // Bit test by & with acc
        uint8_t BIT(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            uint8_t val = registers.acc & systemBus.dataBus;
            registers.setFlagIfZero(val);
            registers.setFlagIfNegative(systemBus.dataBus);

            if ((systemBus.dataBus & 0x40) != 0) {
                registers.setFlag(ProcessorStatus::OverflowFlag);
            } else {
                registers.clearFlag(ProcessorStatus::OverflowFlag);
            }
            registers.setFlagIfNegative(systemBus.dataBus);

            return 0;
        }

        uint8_t CMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            int8_t sub = (int8_t)registers.acc - (int8_t)systemBus.dataBus;
            if (registers.acc == systemBus.dataBus) {
                registers.setFlag(ProcessorStatus::ZeroFlag);
            }
            if (registers.acc >= systemBus.dataBus) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            }

            registers.setFlagIfNegative((uint8_t)sub);
            return 0;
        }

        uint8_t CPY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            int8_t sub = (int8_t)registers.y - (int8_t)systemBus.dataBus;
            if (registers.y == systemBus.dataBus) {
                registers.setFlag(ProcessorStatus::ZeroFlag);
            }
            if (registers.y >= systemBus.dataBus) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            }

            registers.setFlagIfNegative((uint8_t)sub);
            return 0;
        }


        uint8_t CPX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            int8_t sub = (int8_t)registers.x - (int8_t)systemBus.dataBus;
            if (registers.x == systemBus.dataBus) {
                registers.setFlag(ProcessorStatus::ZeroFlag);
            }
            if (registers.x >= systemBus.dataBus) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            }

            registers.setFlagIfNegative((uint8_t)sub);
            return 0;
        }



        /////////////////////////////////////////////////
        //    Stack instructions

        // Copy X to S
        uint8_t TXS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.stackPointer = registers.x;
            return 0;
        }

        // Push accumulator on stack
        uint8_t PHA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.acc;
            pushDataBusToStack(systemBus, registers, memoryMapper);
            return 0;
        }
        // Push processor status on stack
        uint8_t PHP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.statusRegister;
            // Set BRK and unused bit ALWAYs with PHP https://wiki.nesdev.com/w/index.php/Status_flags
            systemBus.dataBus |= 0x30;    
            pushDataBusToStack(systemBus, registers, memoryMapper);
            return 0;
        }

        // copy stack pointer register to x
        uint8_t TSX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x = registers.stackPointer;

            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);
            return 0;
        }

        // Pop accumulator from stack
        uint8_t PLA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.acc = systemBus.dataBus;

            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
            return 0;
        }

        // Pop processor status from stack
        uint8_t PLP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.statusRegister = systemBus.dataBus;
            // Ignore bits 4/5 when pulling from the stack since they are never physically represented in the status register.
            // These indicate the type of irq
            //registers.statusRegister |= 0x30; // just always set.
            return 0;
        }

        /////////////////////////////////////////////////
        // branching

        uint8_t BCC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::CarryFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BCS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::CarryFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BEQ(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::ZeroFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BPL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::NegativeFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BMI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::NegativeFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BNE(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::ZeroFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BVC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::OverflowFlag)) {
                registers.programCounter += (int8_t)systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t BVS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::OverflowFlag)) {
                registers.programCounter += systemBus.dataBus;
                return 1;
            }
            return 0;
        }

        uint8_t JMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.programCounter = systemBus.addressBus;
            return 0;
        }

        /**
            BRK handles general software interrupts as well as level/edge-triggered hardware interrupts (IRQ, NMI).

        */
        uint8_t BRK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.programCounter++;
            interrupt(InterruptType::INT_BRK, systemBus, registers, memoryMapper);
            return 0;
        }

        uint8_t RTI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            // pop stack to processor flags
            popStackSetup(systemBus, registers);
            memoryMapper.doMemoryOperation(systemBus);
            registers.statusRegister = systemBus.dataBus;
            // Ignore bits 4/5  since they should never be set on the physical status register.  They only indicate IRQ source when the 
            // status register was pushed to the stack in the first place.
            registers.statusRegister &= 0xcf;

            // pop program counter
            popStackSetup(systemBus, registers);
            memoryMapper.doMemoryOperation(systemBus);
            registers.setPcl(systemBus.dataBus);
            popStackSetup(systemBus, registers);
            memoryMapper.doMemoryOperation(systemBus);
            registers.setPch(systemBus.dataBus);
            return 0;
        }

        uint8_t JSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.programCounter = systemBus.addressBus;
            // Usual timing order only fetches ADL here
            systemBus.dataBus = registers.pch();
            pushDataBusToStack(systemBus, registers, memoryMapper);
            systemBus.dataBus = registers.pcl();
            pushDataBusToStack(systemBus, registers, memoryMapper);
            // Usual timing order only fetches ADH here
            return 0;
        }

        uint8_t RTS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.setPcl(systemBus.dataBus);
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.setPch(systemBus.dataBus);
            registers.programCounter--;
            return 0;
        }

        uint8_t STA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.acc;
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
            return 0;
        }

        uint8_t STX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.x;
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
            return 0;
        }

        uint8_t STY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.y;
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
            return 0;
        }

        uint8_t UNK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            DBG_CRASH("Unknown instruction encountered %02x: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, sp:$%02x, p: $%02x, pc: $%04x}\n",
                opCode.opCode,
                systemBus.addressBus, systemBus.dataBus, systemBus.read,
                registers.acc, registers.x, registers.y, registers.stackPointer, registers.statusRegister, registers.programCounter);

            return 0;
        }
    }

    //////////////////////////////////////////////////////
    // Utility methods
    // Stack is pushed to from register to memory location $0100 + Stack pointer offset (00-ff)
    // No overflow detection just like the NES

    void pushStackSetup(SystemBus &systemBus, Registers &registers) {
        systemBus.addressBus = (uint16_t)(stackBaseAddress + registers.stackPointer--);
        systemBus.read = false;
    }

    // TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
    void popStackSetup(SystemBus &systemBus, Registers &registers) {
        systemBus.addressBus = (uint16_t)(stackBaseAddress + ++registers.stackPointer);
        systemBus.read = true;
    }

    void popStackToDataBus(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
    }

    void popStackToDataBusWithFlags(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        popStackToDataBus(systemBus, registers, memoryMapper);

        // Update processor status flags
        if (systemBus.dataBus == 0) {
            registers.flagSet(ProcessorStatus::ZeroFlag);
        }
        registers.setFlagIfNegative(systemBus.dataBus);
    }

    void pushDataBusToStack(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        pushStackSetup(systemBus, registers);
        memoryMapper.doMemoryOperation(systemBus);
    }

    uint8_t read(uint16_t addr, SystemBus &systemBus, MemoryMapper &memoryMapper) {
        systemBus.addressBus = addr;
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
        return systemBus.dataBus;
    }

    void interrupt(InterruptType interruptType, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
        if (interruptType == InterruptType::INT_RESET) {
            // do dummy reads
            registers.stackPointer -= 3;
        } else {
            // 1. push program counter
            systemBus.dataBus = registers.pch();
            pushDataBusToStack(systemBus, registers, memoryMapper);
            systemBus.dataBus = registers.pcl();
            pushDataBusToStack(systemBus, registers, memoryMapper);

            // 2. push status register for non-reset situations
            if (interruptType == InterruptType::INT_BRK) {
                // BRK sets BRK bit
                systemBus.dataBus = (registers.statusRegister | 0x30);
            } else {
                systemBus.dataBus = (registers.statusRegister & 0xef) | 0x20;
            }
        }

        static uint16_t interruptVector[4][2] = {
            { 0xfffe, 0xffff },    // IRQ
            { 0xfffa, 0xfffb },    // NMI
            { 0xfffc, 0xfffd },    // RESET
            { 0xfffe, 0xffff },    // BRK
        };
        registers.programCounter = read(interruptVector[interruptType - 1][0], systemBus, memoryMapper);
        registers.programCounter |= (read(interruptVector[interruptType - 1][1], systemBus, memoryMapper) << 8);
    }
}