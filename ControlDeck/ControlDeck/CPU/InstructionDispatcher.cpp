#include "InstructionDispatcher.h"
#include <cstdint>


namespace NES {
    namespace InstructionSet {
        void NOP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            // do nothing
        }

        // signed shift
        void ASL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
        void ROL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }
        }

        void ROR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }
        }

        // Logical shift right (shift right with 0 bit going to carry flag, 7 bit set to 0.)
        void LSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
            } else {
                // write back to memory location
                systemBus.read = false;
                memoryMapper.doMemoryOperation(systemBus);
            }
        }

        void DEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus--;
            registers.setFlagIfZero(systemBus.dataBus);
            registers.setFlagIfNegative(systemBus.dataBus);

            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }

        void INC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus++;
            registers.setFlagIfZero(systemBus.dataBus);
            registers.setFlagIfNegative(systemBus.dataBus);

            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }

        // Decrement X register
        void DEX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x--;
            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);
        }

        // Decrement Y register
        void DEY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y--;
            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);
        }

        void INX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x++;
            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);
        }

        void INY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y++;
            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);
        }

        // Transfer accumulator to x
        void TAX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x = registers.acc;
        }

        // Transfer x to accumulator
        void TXA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = registers.x;
        }

        // Transfer y to accumulator
        void TYA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = registers.y;
        }

        // Transfer accumulator to y
        void TAY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y = registers.acc;
        }

        // Clear carry flag
        void CLC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::CarryFlag);
        }

        // Clear decimal mode flag
        void CLD(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::DecimalMode);
        }

        // Clear interrupt disable flag
        void CLI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::InterruptDisable);

        }

        // Clear overflow flag
        void CLV(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.clearFlag(ProcessorStatus::OverflowFlag);

        }

        // Set carry flag
        void SEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.setFlag(ProcessorStatus::CarryFlag);
        }

        // Set decimal flag
        void SED(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.setFlag(ProcessorStatus::DecimalMode);
        }

        // Set interrupt disable flag
        void SEI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.setFlag(ProcessorStatus::InterruptDisable);

        }

        // TODO .. simplify these
        void ADC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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

        void SBC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
        void EOR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc ^= systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
        }

        void AND(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc &= systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
        }

        void ORA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc |= systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
        }

        void LDA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.acc = systemBus.dataBus;
            registers.setFlagIfZero(registers.acc);
            registers.setFlagIfNegative(registers.acc);
        }

        void LDY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.y = systemBus.dataBus;
            registers.setFlagIfZero(registers.y);
            registers.setFlagIfNegative(registers.y);
        }

        void LDX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.x = systemBus.dataBus;
            registers.setFlagIfZero(registers.x);
            registers.setFlagIfNegative(registers.x);
        }

        // Bit test by & with acc
        void BIT(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            uint8_t val = registers.acc & systemBus.dataBus;
            registers.setFlagIfZero(val);
            registers.setFlagIfNegative(systemBus.dataBus);

            if ((systemBus.dataBus & 0x40) != 0) {
                registers.setFlag(ProcessorStatus::OverflowFlag);
            }
        }

        void CMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            int8_t sub = (int8_t)registers.acc - (int8_t)systemBus.dataBus;
            if (sub == 0) {
                registers.setFlag(ProcessorStatus::ZeroFlag);
            }
            if (sub >= 0) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            }

            registers.setFlagIfNegative((uint8_t)sub);
        }

        void CPY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            int8_t sub = (int8_t)registers.y - (int8_t)systemBus.dataBus;
            if (sub == 0) {
                registers.setFlag(ProcessorStatus::ZeroFlag);
            }
            if (sub >= 0) {
                registers.setFlag(ProcessorStatus::CarryFlag);
            }

            registers.setFlagIfNegative((uint8_t)sub);
        }


        void CPX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
        //    Stack instructions

        // Push x to stack pointer
        void TXS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.x;
            pushDataBusToStack(systemBus, registers, memoryMapper);
        }
        // Push accumulator on stack
        void PHA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.acc;
            pushDataBusToStack(systemBus, registers, memoryMapper);
        }
        // Push processor status on stack
        void PHP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.statusRegister;
            pushDataBusToStack(systemBus, registers, memoryMapper);
        }

        // Pop stack pointer to x
        void TSX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.x = systemBus.dataBus;
        }

        // Pop accumulator from stack
        void PLA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.acc = systemBus.dataBus;
        }

        // Pop processor status from stack
        void PLP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.statusRegister = systemBus.dataBus;
            // Ignore bits 4/5 when pulling from the stack since they are never physically represented in the status register.
            // These indicate the type of irq
            registers.statusRegister &= 0xcf;
        }

        void BCC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::CarryFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BCS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::CarryFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BEQ(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::ZeroFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BPL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::ZeroFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BMI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::NegativeFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BNE(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::NegativeFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BVC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (!registers.flagSet(ProcessorStatus::OverflowFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void BVS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            if (registers.flagSet(ProcessorStatus::OverflowFlag)) {
                registers.programCounter += systemBus.dataBus;
            }
        }

        void JMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.programCounter = systemBus.addressBus;
        }

        /**
            BRK handles general software interrupts as well as level/edge-triggered hardware interrupts (IRQ, NMI).

        */
        void BRK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            // Technically RESET does these operations but with the data bus set to read instead of write.  Leaving them out 
            // unless I feel like accuracy of the reset operation is important later.
            // See: http://www.pagetable.com/?p=410
            if (registers.interruptStatus != InterruptLevel::RESET) {
                // 1. push program counter
                systemBus.dataBus = registers.pch();
                pushDataBusToStack(systemBus, registers, memoryMapper);
                systemBus.dataBus = registers.pcl();
                pushDataBusToStack(systemBus, registers, memoryMapper);
                // 2. push status register
                systemBus.dataBus = registers.statusRegister;

                // The pushed status register reflects the source of the interrupt.
                // Usually the interrupt handler would read this value and test bit 4 to see if the interrupt was software (1) or
                // hardware (0) to then determine the vector address for the handler.
                if (registers.interruptStatus == InterruptLevel::IRQ || registers.interruptStatus == InterruptLevel::NMI) {
                    // set bit 4/5 to 10
                    systemBus.dataBus &= 0xef;    // clear bit 4
                    systemBus.dataBus |= (uint8_t)0x20;    // set bit 5
                } else {
                    // bit 4/5 set to 1
                    systemBus.dataBus |= (uint8_t)0x30;
                }
                pushDataBusToStack(systemBus, registers, memoryMapper);
            }


            static uint16_t interruptVector[3][2] = {
                {0xfffe, 0xffff},    // IRQ, BRK
                {0xfffa, 0xfffb},    // NMI
                {0xfffc, 0xfffd}    // RESET
            };
            // 3. Load PC from address at interrupt vector
            int vector = 0;
            if (registers.interruptStatus == InterruptLevel::NMI) {
                vector = 1;
            } else if (registers.interruptStatus == InterruptLevel::RESET) {
                vector = 2;
            }

            systemBus.addressBus = interruptVector[vector][0];
            systemBus.read = true;
            memoryMapper.doMemoryOperation(systemBus);
            uint8_t adl = systemBus.dataBus;

            systemBus.addressBus = interruptVector[vector][1];
            systemBus.read = true;
            memoryMapper.doMemoryOperation(systemBus);

            // jump to interrupt vector
            registers.programCounter = (systemBus.dataBus << 8) + adl;
        }

        void RTI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
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
        }

        void JSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            registers.programCounter = systemBus.addressBus;
            // Usual timing order only fetches ADL here
            systemBus.dataBus = registers.pch();
            pushDataBusToStack(systemBus, registers, memoryMapper);
            systemBus.dataBus = registers.pcl();
            pushDataBusToStack(systemBus, registers, memoryMapper);
            // Usual timing order only fetches ADH here
        }

        void RTS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.setPcl(systemBus.dataBus);
            popStackToDataBus(systemBus, registers, memoryMapper);
            registers.setPch(systemBus.dataBus);

            // PC needs to be incremented by 1 to get next isntruction
        }

        void STA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.acc;
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }

        void STX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.x;
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }

        void STY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            systemBus.dataBus = registers.y;
            systemBus.read = false;
            memoryMapper.doMemoryOperation(systemBus);
        }


        void UNK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper) {
            // TODO do something to stop execution if we get here?
        }


        //////////////////////////////////////////////////////
        // Utility methods
        // Stack is pushed to from register to memory location $0100 + Stack pointer offset (00-ff)
        // No overflow detection just like the NES

        void pushStackSetup(SystemBus &systemBus, Registers &registers) {
            systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
            registers.stackPointer += 8;
            systemBus.read = false;
        }

        // TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
        void popStackSetup(SystemBus &systemBus, Registers &registers) {
            systemBus.addressBus = (uint16_t)(0x100 + registers.stackPointer);
            registers.stackPointer -= 8;
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
    }
}