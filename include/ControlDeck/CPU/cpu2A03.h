#pragma once
#include "AddressingMode.h"
#include "SystemComponents.h"
#include "InstructionSet.h"
#include "../cartridge.h"
#include "../PPU/PPU2C02.h"


// http://users.telenet.be/kim1-6502/6502/hwman.html - general hardware manual source
namespace NES {
    // constants

    class Cpu2a03;
    struct OpCode;
    typedef uint8_t(*InstructionFnPtr)(const OpCode &opCode, Cpu2a03 &cpu);

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

    struct DebugState {
        const OpCode *opCode;
        uint8_t addressingCycles;
        uint8_t branchCycles;

        uint8_t opCodeArgs[2];

        SystemBus systemBusBefore;
        Registers registersBefore;
        SystemBus systemBusAfter;
        Registers registersAfter;

        DMAData dmaBefore;
        DMAData dmaAfter;
        bool isDma{ false };
        void print();
    };

    // debug info about arguments for an op code
    struct OpCodeArgs {
        uint8_t pagingInstructions{ 0 };
        uint8_t args[2]{};
        // TODO also add indirectly loaded args?
    };


    /**
    * NTSC 6502 CPU 
    */
    class Cpu2a03 {
    public:
        Cpu2a03() = default;
        Cpu2a03(Ppu2C02 *ppu, Cartridge *cartridge);

        DebugState processInstruction();

        //Map memory from the CPU address space, to RAM, PPU, APU, and cartridge components.
        unsigned int doMemoryOperation();
        uint8_t readFromAddress(uint16_t addr);

        /**
        *    Handler for instruction based parameter fetching and calculation of addresses.  Each instruction has an
        *    implied mode of memory access that determines the number of operands to fetch after the op code, and what
        *    the final memory locations an instruction acts on.
        *
        *    Each method calculates the final absolute address in memory to be used by the instruction.  No
        *   read or write is done on that address except for Immediate and Relative addressing, which puts the value on the data bus.
        *
        *    References for addressing implementation: http://nesdev.com/NESDoc.pdf Appendix E
        *   Cycle timing information sourced from http://users.telenet.be/kim1-6502/6502/hwman.html Appendix D
        */
        OpCodeArgs handleAddressingMode(AddressingMode addressingMode);

        void setPowerUpState();
        
        // hardware interrupt signals
        void reset();
        void setIrq();
        void setNmi();

        void interrupt(InterruptType interruptType);

        //utilities
        void pushStackSetup();
        void popStackSetup();
        void popStackToDataBusWithFlags();
        void popStackToDataBus();
        void pushDataBusToStack();
        void synchronizeProcessors();

        SystemRam ram{};
        SystemBus systemBus{};
        Registers registers{};
        DMAData dmaData{};
        Ppu2C02 *ppu{ nullptr };
        Cartridge *cartridge{ nullptr };
        bool debug{ false };
    protected:
        uint32_t cycle{ 0 };
        void waitForNextInstruction();
        // Run cycles of PPU corresponding to a single cpu instruction having occurred
        

        void systemRamHandler(SystemBus &systemBus);
        void ppuRegisterHandler(SystemBus &systemBus);
        void ioRegisterHandler(SystemBus &systemBus);

        // Fetch next op code or handle interrupt
        const OpCode *fetchOpCode();

        ///////////////////////////////////////////////////////////////////////
        // Addressing mode handlers
        
        // TODO use this instead of stack specific instructions which have implied addressing mode.  Make any instruction use addressing mode 
        //        handler to prepare for the next cycle by fetching op code operands??
        void getImmediateAddress(OpCodeArgs &args);
        void getZeroPageAddress(OpCodeArgs &args);
        void getXIndexedZeroPageAddress(OpCodeArgs &args);
        void getYIndexedZeroPageAddress(OpCodeArgs &args);
        // Put full 16 bit address on address bus.
        void getAbsoluateAddress(OpCodeArgs &args);
        void getXIndexedAbsoluteAddress(OpCodeArgs &args);
        void getYIndexedAbsoluteAddress(OpCodeArgs &args);
        void getIndirectAddress(OpCodeArgs &args);
        void getRelativeAddress(OpCodeArgs &args);
        void getXIndexedIndirectAddress(OpCodeArgs &args);
        void getIndirectYIndexedAddress(OpCodeArgs &args);

        // Addressing mode access utilities
        void  fetchIndirectAddressToBus();
        void  fetchAddressFromPCToBus(OpCodeArgs &args);
    };


    const uint16_t stackBaseAddress = 0x100;    // was extern

    namespace InstructionSet {
        uint8_t NOP(const OpCode &opCode, Cpu2a03 &cpu);

        // Read Modify Write

        uint8_t DEC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t INC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t ROR(const OpCode &opCode, Cpu2a03 &cpu);

        // shift (with acc addressing mode)
        uint8_t ASL(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t ROL(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t LSR(const OpCode &opCode, Cpu2a03 &cpu);

        // decrement
        uint8_t DEX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t DEY(const OpCode &opCode, Cpu2a03 &cpu);


        // accumulator transfer
        uint8_t TAX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t TYA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t TXA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t TAY(const OpCode &opCode, Cpu2a03 &cpu);

        // Processor status register set/clear
        uint8_t CLC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t CLD(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t CLI(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t CLV(const OpCode &opCode, Cpu2a03 &cpu);

        uint8_t INX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t SEC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t INY(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t SED(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t SEI(const OpCode &opCode, Cpu2a03 &cpu);

        // internally executed instructions on memory data
        uint8_t ADC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t CMP(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t EOR(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t LDY(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t AND(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t CPX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t LDA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t ORA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BIT(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t CPY(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t LDX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t SBC(const OpCode &opCode, Cpu2a03 &cpu);

        // Stack operations
        uint8_t TXS(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t PHA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t PHP(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t TSX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t PLA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t PLP(const OpCode &opCode, Cpu2a03 &cpu);

        // Branching
        uint8_t BCC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BCS(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BEQ(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BVC(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BVS(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BNE(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BPL(const OpCode &opCode, Cpu2a03 &cpu);

        uint8_t BRK(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t BMI(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t JMP(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t RTI(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t JSR(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t RTS(const OpCode &opCode, Cpu2a03 &cpu);

        uint8_t STA(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t STX(const OpCode &opCode, Cpu2a03 &cpu);
        uint8_t STY(const OpCode &opCode, Cpu2a03 &cpu);

        uint8_t UNK(const OpCode &opCode, Cpu2a03 &cpu);

        // Sourced from http://e-tradition.net/bytes/6502/6502_instruction_set.html
        //static const OpCode opCodes[256];
        const OpCode opCodes[256] = {
            // op, bytes, cycles, page, instruction, addressing mode, handler
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
    }
}

