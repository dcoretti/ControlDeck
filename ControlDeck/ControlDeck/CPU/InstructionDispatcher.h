#pragma once

#include "AddressingModeHandler.h"
#include "InstructionSet.h"
#include "MemoryMapper.h"
#include "SystemComponents.h"
#include <unordered_map>

namespace NES {
    struct OpCode;
    typedef void(*InstructionFnPtr)(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
    
    struct OpCode {
        // Op code reference
        uint8_t opCode;

        // Bytes to read for instruction data
        uint8_t bytes;

        // All instructions are at least 2 cycles long.  First cycle is always to increment and select the program counter as the next address to appear on the bus
        uint8_t cycles;
        // Additional cycles added for page boundaries.
        // 0,1,2 1 for page boundary crossed, 2 for branch sending to different page
        uint8_t pageCycles;

        Instruction instruction;
        AddressingMode addressingMode;
        InstructionFnPtr instructionHandler;
    };
    
    class InstructionDispatcher {
    public:
        // Requires systembus to be set up with the read of the instruction 
        static void dispatchInstruction(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

  
        static void NOP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Read Modify Write
                  
        static void DEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void INC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void ROR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // shift (with acc addressing mode)
        static void ASL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void ROL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void LSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // decrement
        static void DEX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void DEY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);


        // accumulator transfer
        static void TAX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void TYA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void TXA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void TAY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Processor status register set/clear
        static void CLC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void CLD(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void CLI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void CLV(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        static void INX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void SEC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void INY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void SED(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void SEI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // internally executed instructions on memory data
        static void ADC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void CMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void EOR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void LDY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void AND(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void CPX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void LDA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void ORA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BIT(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void CPY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void LDX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void SBC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Stack operations
        static void TXS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void PHA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void PHP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void TSX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void PLA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void PLP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Branching
        static void BCC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BCS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BEQ(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BVC(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BVS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BNE(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BPL(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        static void BRK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void BMI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void JMP(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void RTI(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void JSR(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void RTS(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        static void STA(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void STX(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void STY(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        static void UNK(const OpCode &opCode, SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        //utilities
        static void pushStackSetup(SystemBus &systemBus, Registers &registers);
        static void popStackSetup(SystemBus &systemBus, Registers &registers);
        static void popStackToDataBusWithFlags(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);
        static void popStackToDataBus(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        static void pushDataBusToStack(SystemBus &systemBus, Registers &registers, MemoryMapper& memoryMapper);

        // Sourced from http://e-tradition.net/bytes/6502/6502_instruction_set.html
        static const OpCode opCodes[256];
    };
}
