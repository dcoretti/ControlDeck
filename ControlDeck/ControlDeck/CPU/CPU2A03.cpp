#include "cpu2A03.h"
#include "AddressingModeHandler.h"
namespace NES {
    /**
    *    CPU
    */
    Cpu2a03::Cpu2a03(MemoryMapper *memoryMapper, SystemRam *ram, SystemBus *systemBus, Registers *registers, DMAData *dmaData) :
        memoryMapper(memoryMapper), ram(ram), registers(registers), systemBus(systemBus), dmaData(dmaData) {
    }

    const OpCode * Cpu2a03::fetchOpCode() {
        systemBus->addressBus = registers->programCounter;
        systemBus->read = true;
        memoryMapper->doMemoryOperation(*systemBus);
        registers->programCounter++;

        return &InstructionSet::opCodes[systemBus->dataBus];
    }



    DebugState Cpu2a03::processInstruction() {
        DebugState debugState = DebugState();
        debugState.dmaBefore = *dmaData;
        uint8_t cyclesTaken = 0;
        if (dmaData->isActive) {
            if (dmaData->cycleCounter == 0  || (dmaData->cycleCounter == 1 && cycle & 1)) {
                // do nothing, skip the first cycle and the second if it occurs on an odd cycle.
                // TODO hard to tell from documentation if the skip happens if the start cycle is odd or if the second cycle is odd
            } else if ((dmaData->cycleCounter & 1) == 0) {
                // read mem
                systemBus->addressBus = (dmaData->baseAddress << 8) + dmaData->bytesWritten;
                systemBus->read = true;
                memoryMapper->doMemoryOperation(*systemBus);
                dmaData->curByteToWrite = systemBus->dataBus;
            } else {
                // ppu write
                systemBus->addressBus = 0x2004;
                systemBus->dataBus = dmaData->curByteToWrite;
                systemBus->read = false;
                memoryMapper->doMemoryOperation(*systemBus);
                dmaData->bytesWritten++;
            }

            dmaData->cycleCounter++;
            if (dmaData->bytesWritten == 256) {
                dmaData->isActive = false;
            }
            debugState.dmaAfter = *dmaData;
            cyclesTaken = 1;
            printf("DMA active\n");
        } else if (registers->interruptStatus != InterruptType::INT_NONE) {
            printf("Interrupt being handled for interruptStatus: %d\n", registers->interruptStatus);
            interrupt(registers->interruptStatus, *systemBus, *registers, *memoryMapper);

        }  else {
            // Read the next op code from memory
            const OpCode *opCode = fetchOpCode();
            debugState.opCode = opCode;
            debugState.registersBefore = *registers;
            debugState.systemBusBefore = *systemBus;
            DBG_ASSERT(opCode->instruction != Instruction::UNK, "Unknown instruction encountered %d", opCode->opCode);


            // Set up system bus to contain relevant memory data for a particular instruction.
            AddressingModeHandler::OpCodeArgs opCodeArgs = AddressingModeHandler::handleAddressingMode(opCode->addressingMode, *systemBus, *registers, *memoryMapper);
            debugState.opCodeArgs[0] = opCodeArgs.args[0];
            debugState.opCodeArgs[1] = opCodeArgs.args[1];

            // Call the instruction handler
            uint8_t branchCycles = opCode->instructionHandler(*opCode, *systemBus, *registers, *memoryMapper);

            debugState.registersAfter = *registers;
            debugState.systemBusAfter = *systemBus;

            cyclesTaken += opCode->cycles + branchCycles + opCodeArgs.pagingInstructions;
            debugState.print();
        }
        // clear interrupt source flag set by hardware pins if any.
        registers->interruptStatus = InterruptType::INT_NONE;
        cycle += cyclesTaken;
        return debugState;
    }

    void Cpu2a03::waitForNextInstruction() {
        // implement per instruction wait.  
    }

    //https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
    void Cpu2a03::setPowerUpState() {
        registers->statusRegister = 0x34;   // interrupt enabled with B/unused flag also set (though not used)
        registers->acc = 0;
        registers->x = 0;
        registers->y = 0;
        registers->stackPointer = 0;// during reset gets -=2 to 0xfd;
        registers->programCounter = 0xffed;  // will do reset vector following
        registers->interruptStatus = InterruptType::INT_RESET;
        memset(ram->ram, 0, SystemRam::systemRAMBytes);

    }

    void Cpu2a03::setIrq() {
        registers->interruptStatus = InterruptType::INT_IRQ;
    }
    void Cpu2a03::setNmi() {
        registers->interruptStatus = InterruptType::INT_NMI;
    }

    // $4011 set to 0
    // reset preserves all registers except pc
    // reset vector is at FFFC,FFFD (usually ROM)
    // https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
    void Cpu2a03::reset() {
        registers->interruptStatus = InterruptType::INT_RESET;
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
}