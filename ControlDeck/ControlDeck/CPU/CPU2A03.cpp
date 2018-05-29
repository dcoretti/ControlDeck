#include "cpu2A03.h"
#include "AddressingModeHandler.h"
#include <string.h>
namespace NES {
    Cpu2a03::Cpu2a03(Ppu2C02 *ppu, Cartridge *cartridge) : ppu(ppu), cartridge(cartridge) {
    }

    const OpCode * Cpu2a03::fetchOpCode() {
        systemBus.addressBus = registers.programCounter;
        systemBus.read = true;
        doMemoryOperation();
        registers.programCounter++;

        return &InstructionSet::opCodes[systemBus.dataBus];
    }

    DebugState Cpu2a03::processInstruction() {
        DebugState debugState = DebugState();
        debugState.dmaBefore = dmaData;
        uint8_t cyclesTaken = 0;
        if (dmaData.isActive) {
            if (dmaData.cycleCounter == 0  || (dmaData.cycleCounter == 1 && cycle & 1)) {
                // do nothing, skip the first cycle and the second if it occurs on an odd cycle.
                // TODO hard to tell from documentation if the skip happens if the start cycle is odd or if the second cycle is odd
            } else if ((dmaData.cycleCounter & 1) == 0) {
                // read mem
                systemBus.addressBus = (dmaData.baseAddress << 8) + dmaData.bytesWritten;
                systemBus.read = true;
                doMemoryOperation();
                dmaData.curByteToWrite = systemBus.dataBus;
            } else {
                // ppu write
                systemBus.addressBus = 0x2004;
                systemBus.dataBus = dmaData.curByteToWrite;
                systemBus.read = false;
                doMemoryOperation();
                dmaData.bytesWritten++;
            }

            dmaData.cycleCounter++;
            if (dmaData.bytesWritten == 256) {
                dmaData.isActive = false;
            }
            debugState.dmaAfter = dmaData;
            cyclesTaken = 1;
        } else {
            if (ppu->pollNMI()) {
                printf("NMI\n");
                registers.interruptStatus = InterruptType::INT_NMI;
            }
            // TODO add irq polling?  check interrupt disable flag to ignore IRQ if 1

            if (registers.interruptStatus != InterruptType::INT_NONE) {
                printf("Interrupt being handled for interruptStatus: %d\n", registers.interruptStatus);
                interrupt(registers.interruptStatus);
            } else {
                //            DBG_ASSERT(!registers.flagSet(ProcessorStatus::BreakCommand), "BRK probably shouldn't be set since it isn't used much in nes game ......");
                // Read the next op code from memory
                const OpCode *opCode = fetchOpCode();
                debugState.opCode = opCode;
                debugState.registersBefore = registers;
                debugState.systemBusBefore = systemBus;
                DBG_ASSERT(opCode->instruction != Instruction::UNK, "Unknown instruction encountered %02x at addr %04x", opCode->opCode, registers.programCounter);


                // Set up system bus to contain relevant memory data for a particular instruction.
                OpCodeArgs opCodeArgs = handleAddressingMode(opCode->addressingMode);
                debugState.opCodeArgs[0] = opCodeArgs.args[0];
                debugState.opCodeArgs[1] = opCodeArgs.args[1];

                // Call the instruction handler
                uint8_t branchCycles = opCode->instructionHandler(*opCode, *this);

                debugState.registersAfter = registers;
                debugState.systemBusAfter = systemBus;

                cyclesTaken += opCode->cycles + branchCycles + opCodeArgs.pagingInstructions;

                if (debug) {
                    debugState.print();
                }
            }
        } 
        // clear interrupt source flag set by hardware pins if any.
        registers.interruptStatus = InterruptType::INT_NONE;
        cycle += cyclesTaken;
        return debugState;
    }

    void Cpu2a03::waitForNextInstruction() {
        // implement per instruction wait.  
    }

    //https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
    void Cpu2a03::setPowerUpState() {
        registers.statusRegister = 0x24;   // interrupt enabled with unused flag also set (though not used)
        registers.acc = 0;
        registers.x = 0;
        registers.y = 0;
        registers.stackPointer = 0;// during reset gets -=2 to 0xfd;
        registers.programCounter = 0xffed;  // will do reset vector following
        registers.interruptStatus = InterruptType::INT_RESET;
        memset(ram.ram, 0, SystemRam::systemRAMBytes);

    }

    void Cpu2a03::setIrq() {
        registers.interruptStatus = InterruptType::INT_IRQ;
    }
    void Cpu2a03::setNmi() {
        registers.interruptStatus = InterruptType::INT_NMI;
    }

    // $4011 set to 0
    // reset preserves all registers except pc
    // reset vector is at FFFC,FFFD (usually ROM)
    // https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
    void Cpu2a03::reset() {
        registers.interruptStatus = InterruptType::INT_RESET;
    }

    void DebugState::print(){
        if (isDma) {
            printf("DMA B: {baseAddress: $%04x, isActive: %d, cycleCounter: %d, bytesWritten: %d, curByteToWrite %d\n",
                dmaBefore.baseAddress, dmaBefore.isActive, dmaBefore.cycleCounter, dmaBefore.bytesWritten, dmaBefore.curByteToWrite);
            printf("DMA A: {baseAddress: $%04x, isActive: %d, cycleCounter: %d, bytesWritten: %d, curByteToWrite %d\n",
                dmaAfter.baseAddress, dmaAfter.isActive, dmaAfter.cycleCounter, dmaAfter.bytesWritten, dmaAfter.curByteToWrite);
        } else {
            if (opCode->bytes == 3) {
                // absolute address
                printf("[%s(%02x) $%02x%02x, addrMode: %s]:\n", instructionNames[opCode->instruction], opCode->opCode, opCodeArgs[1], opCodeArgs[0], addressingModeNames[opCode->addressingMode]);
            } else if (opCode->bytes == 2) {
                // single arg
                printf("[%s(%02x), $%02x, addrMode: %s]:\n", instructionNames[opCode->instruction], opCode->opCode, opCodeArgs[0], addressingModeNames[opCode->addressingMode]);

            } else {
                printf("[%s(%02x), addrMode: %s]:\n",  instructionNames[opCode->instruction], opCode->opCode, addressingModeNames[opCode->addressingMode]);
            }


            printf("B: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, p: $%02x, sp:$%02x, pc: $%04x}\n",
                systemBusBefore.addressBus, systemBusBefore.dataBus, systemBusBefore.read,
                registersBefore.acc, registersBefore.x, registersBefore.y, registersBefore.statusRegister, registersBefore.stackPointer, registersBefore.programCounter);
            printf("A: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, p: $%02x, sp: $%02x, pc: $%04x}\n",
                systemBusAfter.addressBus, systemBusAfter.dataBus, systemBusAfter.read,
                registersAfter.acc, registersAfter.x, registersAfter.y, registersAfter.statusRegister, registersAfter.stackPointer, registersAfter.programCounter);
        }
    }

    void Cpu2a03::synchronizeProcessors() {
        ppu->doPpuCycle();
        ppu->doPpuCycle();
        ppu->doPpuCycle();
    }


    /*
    Source :http://nesdev.com/NESDoc.pdf Appendix D for memory mapper functions
    $1000
    $c000    PRG-ROM (upper)
    $8000    PRG-ROM (lower)
    $6000    Save RAM
    $4020    Expansion ROM
    $4000    I/O Registers
    $2008
    $2000    IO Registers

    mirrors of ram...

    -----------------------

    $0200    Main RAM
    $0100    Stack
    $0000    Zero Page
    */
    unsigned int Cpu2a03::doMemoryOperation() {
        synchronizeProcessors();


        // 2kb system ram, mirrored 3 additional times
        if (systemBus.addressBus < 0x2000) {
            // 0x0800-0x0FFF  mirror 1
            // 0x1000-0x17FF  mirror 2
            // 0x1800-0x1FFF  mirror 3
            systemRamHandler(systemBus);
        }
        // $200-$3fff PPU registers (8bytes) mirrors of those 8 bytes for a total of 8kb
        else if (systemBus.addressBus < 0x4000) {
            //Eight bytes of memory mapped PPU registers mirrored
            ppuRegisterHandler(systemBus);
        }
        // I/O registers, APU registers
        else if (systemBus.addressBus < 0x4020) {
            // TODO add APU etc

            if (systemBus.addressBus == 0x4014 && !systemBus.read) {
                // Activate DMA for processor to take over.
                dmaData.activate(systemBus.dataBus);
            }
        }
        // General cartrige space including PRG ROM/RAM, SRAM/WRAM (save data), mapper registers, etc.
        else {
            cartridge->mmc->doMemoryOperation(systemBus, *cartridge);
        }

        // TODO memory cycles cost by memory op? 
        return 0;
    }

    uint8_t Cpu2a03::readFromAddress(uint16_t addr) {
        systemBus.addressBus = addr;
        systemBus.read = true;
        doMemoryOperation();
        return systemBus.dataBus;
    }

    void Cpu2a03::ioRegisterHandler(SystemBus &systemBus) {
        DBG_ASSERT(systemBus.addressBus < 0x4020 && systemBus.addressBus > 0x4000, "invalid address for io handler %d", systemBus.addressBus);

    }

    void Cpu2a03::ppuRegisterHandler(SystemBus &systemBus) {
        uint16_t actualAddr = 0x2000 + ((systemBus.addressBus - 0x2000) % 8);

        PPURegister reg = (PPURegister)(actualAddr - 0x2000);
        if (systemBus.read) {
            systemBus.dataBus = ppu->readRegister(reg);
        } else {
            ppu->writeRegister(reg, systemBus.dataBus);
        }
    }

    // Read and write main CPU memory.  dataBusBuffer holds intermediate read value to be read
    // by instruction into register etc.
    void Cpu2a03::systemRamHandler(SystemBus &systemBus) {
        size_t actual = systemBus.addressBus % 0x800;
        if (systemBus.read) {
            systemBus.dataBus = ram.ram[actual];
        } else {
            ram.ram[actual] = systemBus.dataBus;
        }
    }


    //////////////////////////////////////////////////////
    // Utility methods
    // Stack is pushed to from register to memory location $0100 + Stack pointer offset (00-ff)
    // No overflow detection just like the NES

    void Cpu2a03::pushStackSetup() {
        systemBus.addressBus = (uint16_t)(stackBaseAddress + registers.stackPointer--);
        systemBus.read = false;
        synchronizeProcessors();    // for registers.stackPointer-- 
    }

    // TODO a bit confusing where push knows about register x -> addr  whereas this one only knows where to read.  Does that matter?
    void Cpu2a03::popStackSetup() {
        systemBus.addressBus = (uint16_t)(stackBaseAddress + ++registers.stackPointer);
        systemBus.read = true;
        synchronizeProcessors();    // for ++registers.stackPointer 
    }

    void Cpu2a03::popStackToDataBus() {
        popStackSetup();
        doMemoryOperation();
    }

    void Cpu2a03::popStackToDataBusWithFlags() {
        popStackToDataBus();

        // Update processor status flags
        if (systemBus.dataBus == 0) {
            registers.flagSet(ProcessorStatus::ZeroFlag);
        }
        registers.setFlagIfNegative(systemBus.dataBus);
    }

    void Cpu2a03::pushDataBusToStack() {
        pushStackSetup();
        doMemoryOperation();
    }

    void Cpu2a03::interrupt(InterruptType interruptType) {
        if (interruptType == InterruptType::INT_RESET) {
            // do dummy reads
            synchronizeProcessors();   
            synchronizeProcessors(); 
            synchronizeProcessors(); 

            registers.stackPointer -= 3;
        } else {
            // 1. push program counter
            systemBus.dataBus = registers.pch();
            pushDataBusToStack();
            systemBus.dataBus = registers.pcl();
            pushDataBusToStack();

            // 2. push status register for non-reset situations
            if (interruptType == InterruptType::INT_BRK) {
                // BRK sets BRK bit
                systemBus.dataBus = (registers.statusRegister | 0x30);
            } else {
                systemBus.dataBus = (registers.statusRegister & 0xef) | 0x20;
            }
            pushDataBusToStack();
        }

        static uint16_t interruptVector[4][2] = {
            { 0xfffe, 0xffff },    // IRQ
            { 0xfffa, 0xfffb },    // NMI
            { 0xfffc, 0xfffd },    // RESET
            { 0xfffe, 0xffff },    // BRK
        };
        registers.programCounter = readFromAddress(interruptVector[interruptType - 1][0]);
        registers.programCounter |= (readFromAddress(interruptVector[interruptType - 1][1]) << 8);
    }

    OpCodeArgs Cpu2a03::handleAddressingMode(AddressingMode addressingMode) {
        OpCodeArgs opCodeArgs = OpCodeArgs();
        switch (addressingMode) {
        case AddressingMode::Absolute:
            getAbsoluateAddress(opCodeArgs);
            break;
        case AddressingMode::AbsoluteX:
            getXIndexedAbsoluteAddress(opCodeArgs);
            break;
        case AddressingMode::AbsoluteY:
            getYIndexedAbsoluteAddress(opCodeArgs);
            break;
        case AddressingMode::Accumulator:
            // no memory access used
            return opCodeArgs;
        case AddressingMode::Immediate:
            getImmediateAddress(opCodeArgs);
            break;
        case AddressingMode::Implicit:
            // no memory access used
            return opCodeArgs;
        case AddressingMode::Indirect:
            getIndirectAddress(opCodeArgs);
            break;
        case AddressingMode::IndirectYIndexed:
            getIndirectYIndexedAddress(opCodeArgs);
            break;
        case AddressingMode::Relative:
            getRelativeAddress(opCodeArgs);
            break;
        case AddressingMode::XIndexedIndirect:
            getXIndexedIndirectAddress(opCodeArgs);
            break;
        case AddressingMode::ZeroPage:
            getZeroPageAddress(opCodeArgs);
            break;
        case AddressingMode::ZeroPageX:
            getXIndexedZeroPageAddress(opCodeArgs);
            break;
        case AddressingMode::ZeroPageY:
            getYIndexedZeroPageAddress(opCodeArgs);
            break;
        case AddressingMode::Undefined:
        default:
            DBG_CRASH("Invalid address mode encountered for instruction %d", addressingMode);
            return opCodeArgs;
        }

        // Do final read
        //        systemBus.read = true;
        //        memoryMapper.doMemoryOperation(systemBus);

        // Fix program counter
        registers.programCounter += addressingModeProgramCounterDelta[addressingMode];

        return opCodeArgs;
    }

    /**
    *    Operate on data directly held in instruction operand
    *    1 Cycle:
    *        1. Get data from PC+1
    */
    void Cpu2a03::getImmediateAddress(OpCodeArgs &args) {
        args.args[0] = (registers.programCounter);
    }

    /**
    *    Branch specific instructions need to fetch the next instruction.
    *    1 Cycle:
    *        1. Fetch operand for branch jump to be used when determining conditional
    */
    void Cpu2a03::getRelativeAddress(OpCodeArgs &args) {
        // Argument is fetched to be used to alter PC
        args.args[0] = readFromAddress(registers.programCounter);
    }

    /**
    *    Get address in zero page ($0000-$00ff)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void Cpu2a03::getZeroPageAddress(OpCodeArgs &args) {
        // get operand addr
        readFromAddress(registers.programCounter);
        args.args[0] = systemBus.dataBus;

        // zero page addr only needs lower byte
        readFromAddress((uint16_t)systemBus.dataBus);
    }

    /**
    *    Get address in zero page ($0000-$00ff) + X (with wraparound in zero page!)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void Cpu2a03::getXIndexedZeroPageAddress(OpCodeArgs &args) {
        readFromAddress(registers.programCounter);
        args.args[0] = systemBus.dataBus;

        readFromAddress((systemBus.dataBus + registers.x) % 0x80);
    }

    /**
    *    Get address in zero page ($0000-$00ff) + Y (with wraparound in zero page!)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void Cpu2a03::getYIndexedZeroPageAddress(OpCodeArgs &args) {
        readFromAddress(registers.programCounter);
        args.args[0] = systemBus.dataBus;

        readFromAddress((systemBus.dataBus + registers.y) % 0x80);
    }

    /**
    *    2 Cycles:
    *        1. Fetch ADL
    *        2. Fetch ADH
    */
    void Cpu2a03::fetchAddressFromPCToBus(OpCodeArgs &args) {
        readFromAddress(registers.programCounter);
        uint8_t adlTmp = systemBus.dataBus;
        readFromAddress(registers.programCounter + 1);
        systemBus.setAddressBus(adlTmp, systemBus.dataBus);

        args.args[0] = adlTmp;
        args.args[1] = systemBus.dataBus;
    }

    void Cpu2a03::getAbsoluateAddress(OpCodeArgs &args) {
        fetchAddressFromPCToBus(args);
        systemBus.read = true;
        doMemoryOperation();
    }

    void Cpu2a03::getXIndexedAbsoluteAddress(OpCodeArgs &args) {
        fetchAddressFromPCToBus(args);
        if ((systemBus.addressBus & 0xff) + registers.x >= 0xff) {
            // page boundary crossed
            args.pagingInstructions = 1;
        }
        systemBus.addressBus += registers.x;

        systemBus.read = true;
        doMemoryOperation();
    }

    void Cpu2a03::getYIndexedAbsoluteAddress(OpCodeArgs &args) {
        fetchAddressFromPCToBus(args);
        if ((systemBus.addressBus & 0xff) + registers.y >= 0xff) {
            // page boundary crossed
            args.pagingInstructions = 1;
        }
        systemBus.addressBus += registers.y;

        systemBus.read = true;
        doMemoryOperation();
    }


    /**
    *    JMP specific addressing mode: load the address located at the memory location specified by the operands
    *    4 Cycles:
    *        1. Fetch ADL (indirect address)
    *        2. Fetch ADH (indirect address)
    *        3. Fetch Jump L address from indirect address in memory
    *        4. Fetch Jump H address
    */
    void Cpu2a03::getIndirectAddress(OpCodeArgs &args) {
        fetchAddressFromPCToBus(args);

        fetchIndirectAddressToBus();

        systemBus.read = true;
        doMemoryOperation();
    }

    /**
    *    Also called Pre-indexed.  X + operand gives the address of the location of the data
    *    3 Cycles:
    *        1. Fetch ADL (indirect address)
    *        2. Fetch [ADL]+X (location of final address)
    *        3. Fetch ADL+X+1
    */
    void Cpu2a03::getXIndexedIndirectAddress(OpCodeArgs &args) {
        readFromAddress(registers.programCounter);
        args.args[0] = systemBus.dataBus;

        systemBus.setAdlOnly((systemBus.dataBus + registers.x) % 0xff);
        fetchIndirectAddressToBus();
        // address bus now contains the address retrieved from x in zero page.

        systemBus.read = true;
        doMemoryOperation();
    }

    /**
    *    Also called post-indexed.  operand gives a zero page address.  The full address at that location + Y is the final address
    *    3 Cycles:
    *        1. Fetch ADL (zero page pointer address)
    *        2. Fetch ADL from zero page (effective address)
    *        3. Fetch ADH from zero page (effective address) and add Y to ADL.
    *
    *    TODO handle special case denoted in http://www.fceux.com/web/help/fceux.html?6502CPU.html 6th cycle when given invalid effective address
    */
    void Cpu2a03::getIndirectYIndexedAddress(OpCodeArgs &args) {
        readFromAddress(registers.programCounter);
        args.args[0] = systemBus.dataBus;

        systemBus.setAdlOnly(systemBus.dataBus);
        fetchIndirectAddressToBus();
        if ((systemBus.addressBus & 0xff) + registers.y > 0xff) {
            args.pagingInstructions = 1;
        }
        systemBus.addressBus += registers.y;

        systemBus.read = true;
        doMemoryOperation();
    }

    /**
    * Helper for indirect addressing: given a system bus with the location of the indirect address, fetch the 16bit address starting at that location
    * and set it on the systemBus addressBus.
    */
    void Cpu2a03::fetchIndirectAddressToBus() {
        systemBus.read = true;
        doMemoryOperation();
        uint8_t tmpAdl = systemBus.dataBus;
        systemBus.addressBus++; // adh at next word
        doMemoryOperation();

        systemBus.setAddressBus(tmpAdl, systemBus.dataBus);
    }

    // TODO maybe move this into CPU namespace finally to remove the cpu argument?
    namespace InstructionSet {
        uint8_t NOP(const OpCode &opCode, Cpu2a03 &cpu) {
            // do nothing
            return 0;
        }

        // signed shift
        uint8_t ASL(const OpCode &opCode, Cpu2a03 &cpu) {
            uint8_t arg = cpu.systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = cpu.registers.acc;
            }

            if ((arg & 0x80) != 0) {
                cpu.registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                cpu.registers.clearFlag(ProcessorStatus::CarryFlag);
            }
            arg = (int8_t)arg << 1;
            cpu.registers.setFlagIfZero(arg);
            cpu.registers.setFlagIfNegative(arg);
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                cpu.registers.acc = arg;
            } else {
                // write back to memory location
                cpu.systemBus.read = false;
                cpu.doMemoryOperation();
            }

            return 0;
        }

        // Rotate left (0-filled)
        uint8_t ROL(const OpCode &opCode, Cpu2a03 &cpu) {
            uint8_t arg = cpu.systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = cpu.registers.acc;
            }

            bool carrySet = cpu.registers.flagSet(ProcessorStatus::CarryFlag);
            if ((arg & 0x80) != 0) {
                cpu.registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                cpu.registers.clearFlag(ProcessorStatus::CarryFlag);
            }
            arg = arg << (uint8_t)1;
            arg += carrySet ? 1 : 0;

            cpu.registers.setFlagIfZero(arg);
            cpu.registers.setFlagIfNegative(arg);

            if (opCode.addressingMode == AddressingMode::Accumulator) {
                cpu.registers.acc = arg;
            } else {
                // write back to memory location
                cpu.systemBus.read = false;
                cpu.doMemoryOperation();
            }

            return 0;
        }

        uint8_t ROR(const OpCode &opCode, Cpu2a03 &cpu) {
            uint8_t arg = cpu.systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = cpu.registers.acc;
            }
            uint8_t carryMask = cpu.registers.flagSet(ProcessorStatus::CarryFlag) ? 0x80 : 0x00;
            if ((arg & 0x01) != 0) {
                cpu.registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                cpu.registers.clearFlag(ProcessorStatus::CarryFlag);
            }

            arg = arg >> (uint8_t)1;
            // old carry flag is brought in as bit 7;
            arg &= carryMask;

            cpu.registers.setFlagIfZero(arg);
            cpu.registers.setFlagIfNegative(arg);

            if (opCode.addressingMode == AddressingMode::Accumulator) {
                cpu.registers.acc = arg;
            } else {
                // write back to memory location
                cpu.systemBus.read = false;
                cpu.doMemoryOperation();
            }
            return 0;
        }

        // Logical shift right (shift right with 0 bit going to carry flag, 7 bit set to 0.)
        uint8_t LSR(const OpCode &opCode, Cpu2a03 &cpu) {
            uint8_t arg = cpu.systemBus.dataBus;
            if (opCode.addressingMode == AddressingMode::Accumulator) {
                arg = cpu.registers.acc;
            }

            if ((arg & 0x1) != 0) {
                cpu.registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                cpu.registers.clearFlag(ProcessorStatus::CarryFlag);
            }
            arg = arg >> (uint8_t)1;
            cpu.registers.setFlagIfZero(arg);
            cpu.registers.setFlagIfNegative(arg);

            if (opCode.addressingMode == AddressingMode::Accumulator) {
                cpu.registers.acc = arg;
            } else {
                // write back to memory location
                cpu.systemBus.read = false;
                cpu.doMemoryOperation();
            }
            return 0;
        }

        uint8_t DEC(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus--;
            cpu.registers.setFlagIfZero(cpu.systemBus.dataBus);
            cpu.registers.setFlagIfNegative(cpu.systemBus.dataBus);

            cpu.systemBus.read = false;
            cpu.doMemoryOperation();

            return 0;
        }

        uint8_t INC(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus++;
            cpu.registers.setFlagIfZero(cpu.systemBus.dataBus);
            cpu.registers.setFlagIfNegative(cpu.systemBus.dataBus);

            cpu.systemBus.read = false;
            cpu.doMemoryOperation();

            return 0;
        }

        // Decrement X register
        uint8_t DEX(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.x--;
            cpu.registers.setFlagIfZero(cpu.registers.x);
            cpu.registers.setFlagIfNegative(cpu.registers.x);

            return 0;
        }

        // Decrement Y register
        uint8_t DEY(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.y--;
            cpu.registers.setFlagIfZero(cpu.registers.y);
            cpu.registers.setFlagIfNegative(cpu.registers.y);

            return 0;
        }

        uint8_t INX(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.x++;
            cpu.registers.setFlagIfZero(cpu.registers.x);
            cpu.registers.setFlagIfNegative(cpu.registers.x);

            return 0;
        }

        uint8_t INY(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.y++;
            cpu.registers.setFlagIfZero(cpu.registers.y);
            cpu.registers.setFlagIfNegative(cpu.registers.y);

            return 0;
        }

        // Transfer accumulator to x
        uint8_t TAX(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.x = cpu.registers.acc;

            cpu.registers.setFlagIfZero(cpu.registers.x);
            cpu.registers.setFlagIfNegative(cpu.registers.x);

            return 0;
        }

        // Transfer x to accumulator
        uint8_t TXA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc = cpu.registers.x;

            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);

            return 0;
        }

        // Transfer y to accumulator
        uint8_t TYA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc = cpu.registers.y;

            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);

            return 0;
        }

        // Transfer accumulator to y
        uint8_t TAY(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.y = cpu.registers.acc;

            cpu.registers.setFlagIfZero(cpu.registers.y);
            cpu.registers.setFlagIfNegative(cpu.registers.y);

            return 0;
        }

        // Clear carry flag
        uint8_t CLC(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.clearFlag(ProcessorStatus::CarryFlag);

            return 0;
        }

        // Clear decimal mode flag
        uint8_t CLD(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.clearFlag(ProcessorStatus::DecimalMode);

            return 0;
        }

        // Clear interrupt disable flag
        uint8_t CLI(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.clearFlag(ProcessorStatus::InterruptDisable);

            return 0;
        }

        // Clear overflow flag
        uint8_t CLV(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.clearFlag(ProcessorStatus::OverflowFlag);

            return 0;
        }

        // Set carry flag
        uint8_t SEC(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.setFlag(ProcessorStatus::CarryFlag);

            return 0;
        }

        // Set decimal flag
        uint8_t SED(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.setFlag(ProcessorStatus::DecimalMode);

            return 0;
        }

        // Set interrupt disable flag
        uint8_t SEI(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.setFlag(ProcessorStatus::InterruptDisable);

            return 0;
        }

        uint8_t add(uint8_t busVal, Cpu2a03 &cpu) {
            uint16_t add = cpu.registers.acc + busVal + cpu.registers.flagSet(ProcessorStatus::CarryFlag);
            if (add > 0xff) {
                cpu.registers.setFlag(ProcessorStatus::CarryFlag);
            }
            if (cpu.registers.willAddOverflow(busVal)) {
                cpu.registers.setFlag(ProcessorStatus::OverflowFlag);
            }
            cpu.registers.setFlagIfZero((uint8_t)add);
            cpu.registers.setFlagIfNegative((uint8_t)add);
            return (uint8_t)add;
        }

        // TODO .. simplify these
        uint8_t ADC(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc = add(cpu.systemBus.dataBus, cpu);
            return 0;
        }

        uint8_t SBC(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc = add(~cpu.systemBus.dataBus, cpu);
            return 0;
        }

        /////////////////////////////////////////////////
        // Logical operations on accumulator
        uint8_t EOR(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc ^= cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);
            return 0;
        }

        uint8_t AND(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc &= cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);
            return 0;
        }

        uint8_t ORA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc |= cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);
            return 0;
        }

        uint8_t LDA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.acc = cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);
            return 0;
        }

        uint8_t LDY(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.y = cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(cpu.registers.y);
            cpu.registers.setFlagIfNegative(cpu.registers.y);
            return 0;
        }

        uint8_t LDX(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.x = cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(cpu.registers.x);
            cpu.registers.setFlagIfNegative(cpu.registers.x);
            return 0;
        }

        // Bit test by & with acc
        uint8_t BIT(const OpCode &opCode, Cpu2a03 &cpu) {
            uint8_t val = cpu.registers.acc & cpu.systemBus.dataBus;
            cpu.registers.setFlagIfZero(val);
            cpu.registers.setFlagIfNegative(cpu.systemBus.dataBus);

            if ((cpu.systemBus.dataBus & 0x40) != 0) {
                cpu.registers.setFlag(ProcessorStatus::OverflowFlag);
            } else {
                cpu.registers.clearFlag(ProcessorStatus::OverflowFlag);
            }
            cpu.registers.setFlagIfNegative(cpu.systemBus.dataBus);

            return 0;
        }

        void compare(uint8_t reg, uint8_t arg, Cpu2a03 &cpu) {
            uint8_t res = reg - arg;
            cpu.registers.setFlagIfZero(res);

            if (reg >= arg) {
                cpu.registers.setFlag(ProcessorStatus::CarryFlag);
            } else {
                cpu.registers.clearFlag(ProcessorStatus::CarryFlag);
            }

            cpu.registers.setFlagIfNegative(res);
        }

        uint8_t CMP(const OpCode &opCode, Cpu2a03 &cpu) {
            compare(cpu.registers.acc, cpu.systemBus.dataBus, cpu);
            return 0;
        }

        uint8_t CPY(const OpCode &opCode, Cpu2a03 &cpu) {
            compare(cpu.registers.y, cpu.systemBus.dataBus, cpu);
            return 0;
        }


        uint8_t CPX(const OpCode &opCode, Cpu2a03 &cpu) {
            compare(cpu.registers.x, cpu.systemBus.dataBus, cpu);
            return 0;
        }



        /////////////////////////////////////////////////
        //    Stack instructions

        // Copy X to S
        uint8_t TXS(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.stackPointer = cpu.registers.x;
            return 0;
        }

        // Push accumulator on stack
        uint8_t PHA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus = cpu.registers.acc;
            cpu.pushDataBusToStack();
            return 0;
        }
        // Push processor status on stack
        uint8_t PHP(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus = cpu.registers.statusRegister;
            // Set BRK and unused bit ALWAYs with PHP https://wiki.nesdev.com/w/index.php/Status_flags
            cpu.systemBus.dataBus |= 0x30;
            cpu.pushDataBusToStack();
            return 0;
        }

        // copy stack pointer register to x
        uint8_t TSX(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.x = cpu.registers.stackPointer;

            cpu.registers.setFlagIfZero(cpu.registers.x);
            cpu.registers.setFlagIfNegative(cpu.registers.x);
            return 0;
        }

        // Pop accumulator from stack
        uint8_t PLA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.popStackToDataBus();
            cpu.registers.acc = cpu.systemBus.dataBus;

            cpu.registers.setFlagIfZero(cpu.registers.acc);
            cpu.registers.setFlagIfNegative(cpu.registers.acc);
            return 0;
        }

        // Pop processor status from stack
        uint8_t PLP(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.popStackToDataBus();
            cpu.registers.statusRegister = cpu.systemBus.dataBus;
            // Ignore bits 4/5 when pulling from the stack since they are never physically represented in the status register.
            // These indicate the type of irq
            //cpu.registers.statusRegister |= 0x30; // just always set.
            return 0;
        }

        /////////////////////////////////////////////////
        // branching

        inline uint8_t branch(bool condition, Cpu2a03 &cpu) {
            if (condition) {
                if ((cpu.registers.programCounter & 0xff) + (uint8_t)cpu.systemBus.dataBus > 0xff) {
                    // page crossing takes an extra cycle due to dummy read made on both sides of the page ($00/$ff)
                    cpu.synchronizeProcessors();
                }
                cpu.registers.programCounter += (int8_t)cpu.systemBus.dataBus;

                return 1;   // extra cycle to take for branching
            }
            return 0;
        }

        uint8_t BCC(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(!cpu.registers.flagSet(ProcessorStatus::CarryFlag), cpu);
        }

        uint8_t BCS(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(cpu.registers.flagSet(ProcessorStatus::CarryFlag), cpu);
        }

        uint8_t BEQ(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(cpu.registers.flagSet(ProcessorStatus::ZeroFlag), cpu);
        }

        uint8_t BPL(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(!cpu.registers.flagSet(ProcessorStatus::NegativeFlag), cpu);
        }

        uint8_t BMI(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(cpu.registers.flagSet(ProcessorStatus::NegativeFlag), cpu);
        }

        uint8_t BNE(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(!cpu.registers.flagSet(ProcessorStatus::ZeroFlag), cpu);
        }

        uint8_t BVC(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(!cpu.registers.flagSet(ProcessorStatus::OverflowFlag), cpu);
        }

        uint8_t BVS(const OpCode &opCode, Cpu2a03 &cpu) {
            return branch(cpu.registers.flagSet(ProcessorStatus::OverflowFlag), cpu);
        }

        uint8_t JMP(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.programCounter = cpu.systemBus.addressBus;
            return 0;
        }

        /**
        BRK handles general software interrupts as well as level/edge-triggered hardware interrupts (IRQ, NMI).

        */
        uint8_t BRK(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.registers.programCounter++;
            cpu.interrupt(InterruptType::INT_BRK);
            return 0;
        }

        uint8_t RTI(const OpCode &opCode, Cpu2a03 &cpu) {
            // pop stack to processor flags
            cpu.popStackToDataBus();

            cpu.registers.statusRegister = cpu.systemBus.dataBus;
            // Ignore bits 4/5  since they should never be set on the physical status register.  They only indicate IRQ source when the 
            // status register was pushed to the stack in the first place.
            cpu.registers.statusRegister &= 0xcf;

            // pop program counter
            cpu.popStackToDataBus();
            cpu.registers.setPcl(cpu.systemBus.dataBus);
            cpu.popStackToDataBus();
            cpu.registers.setPch(cpu.systemBus.dataBus);
            return 0;
        }

        uint8_t JSR(const OpCode &opCode, Cpu2a03 &cpu) {
            uint16_t jmpAddress = cpu.systemBus.addressBus;
            // Usual timing order only fetches ADL here
            uint16_t ret = cpu.registers.programCounter - 1;
            printf("jmp to %04x with return addr %04x\n", jmpAddress, ret);

            cpu.systemBus.dataBus = ret >> 8;
            cpu.pushDataBusToStack();
            cpu.systemBus.dataBus = ret & 0xff;
            cpu.pushDataBusToStack();

            cpu.registers.programCounter = jmpAddress;
            // Usual timing order only fetches ADH here

            return 0;
        }

        uint8_t RTS(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.popStackToDataBus();
            cpu.registers.setPcl(cpu.systemBus.dataBus);
            cpu.popStackToDataBus();
            cpu.registers.setPch(cpu.systemBus.dataBus);
            cpu.registers.programCounter++;
            return 0;
        }

        uint8_t STA(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus = cpu.registers.acc;
            cpu.systemBus.read = false;
            cpu.doMemoryOperation();
            return 0;
        }

        uint8_t STX(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus = cpu.registers.x;
            cpu.systemBus.read = false;
            cpu.doMemoryOperation();
            return 0;
        }

        uint8_t STY(const OpCode &opCode, Cpu2a03 &cpu) {
            cpu.systemBus.dataBus = cpu.registers.y;
            cpu.systemBus.read = false;
            cpu.doMemoryOperation();
            return 0;
        }

        uint8_t UNK(const OpCode &opCode, Cpu2a03 &cpu) {
            DBG_CRASH("Unknown instruction encountered %02x: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, sp:$%02x, p: $%02x, pc: $%04x}\n",
                opCode.opCode,
                cpu.systemBus.addressBus, cpu.systemBus.dataBus, cpu.systemBus.read,
                cpu.registers.acc, cpu.registers.x, cpu.registers.y, cpu.registers.stackPointer, cpu.registers.statusRegister, cpu.registers.programCounter);

            return 0;
        }
    }

}