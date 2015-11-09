#pragma once

#include "OpCode.h"
#include "AddressingModeHandler.h"
#include "MemoryMapper.h"
#include "SystemComponents.h"
#include <unordered_map>

namespace NES {
    typedef void(*InstructionFnPtr)(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

    class InstructionDispatcher {
    public:
        InstructionDispatcher();

        void dispatchInstruction(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
    private:

        void NOP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Read Modify Write
                  
        void DEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void INC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void ROR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // shift (with acc addressing mode)
        void ASL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void ROL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void LSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // decrement
        void DEX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void DEY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);


        // accumulator transfer
        void TAX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void TYA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void TXA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void TAY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Processor status register set/clear
        void CLC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void CLD(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void CLI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void CLV(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        void INX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void SEC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void INY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void SED(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void SEI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // internally executed instructions on memory data
        void ADC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void CMP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void EOR(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void LDY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void AND(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void CPX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void LDA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void ORA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BIT(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void CPY(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void LDX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void SBC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Stack operations
        void TXS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void PHA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void PHP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void TSX(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void PLA(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void PLP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Branching
        void BCC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BCS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BEQ(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BVC(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BVS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BNE(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BPL(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        void BRK(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void BMI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void JMP(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void RTI(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void JSR(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void RTS(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);




        //utilities
        void pushStackSetup(SystemBus &systemBus, Registers &registers);
        void popStackSetup(SystemBus &systemBus, Registers &registers);
        void popStackToDataBusWithFlags(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        void popStackToDataBus(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        void pushDataBusToStack(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);



        AddressingModeHandler addressingModeHandler;
    };
}
