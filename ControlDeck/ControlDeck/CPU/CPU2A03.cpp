#include "cpu2A03.h"
#include "AddressingModeHandler.h"
namespace NES {
    /**
    *    CPU
    */
    Cpu2a03::Cpu2a03(MemoryMapper *memoryMapper, SystemRam *ram, SystemBus *systemBus, Registers *registers) :
        memoryMapper(memoryMapper), ram(ram), registers(registers), systemBus(systemBus) {
    }

    OpCode Cpu2a03::fetchOpCode() {
        if (registers->interruptStatus == InterruptLevel::NONE) {
            systemBus->addressBus = registers->programCounter;
            systemBus->read = true;
            memoryMapper->doMemoryOperation(*systemBus);
            registers->programCounter++;
        } else {
            // If an interrupt is currently in process, only a NMI can get through
            if (!registers->flagSet(ProcessorStatus::InterruptDisable) || registers->interruptStatus != InterruptLevel::IRQ) {
                // Treat an interrupt as an injected "0" op code (BRK).  
                // Interrupt level will determine the treatment of that op code in the instruction dispatcher
                systemBus->dataBus = 0;
            }
        }
        return InstructionSet::opCodes[systemBus->dataBus];
    }

    struct DebugState {
        OpCode opCode;
        SystemBus systemBusBefore;
        Registers registersBefore;
        SystemBus systemBusAfter;
        Registers registersAfter;

        void print() {
            printf("[%s (%02x), addrMode: %s]:\n",
                instructionNames[opCode.instruction], opCode.opCode, addressingModeNames[opCode.addressingMode]);
            printf("B: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, sp:$%02x, p: $%02x, pc: $%04x}\n",
                systemBusBefore.addressBus, systemBusBefore.dataBus, systemBusBefore.read,
                registersBefore.acc, registersBefore.x, registersBefore.y, registersBefore.stackPointer, registersBefore.statusRegister, registersBefore.programCounter);
            printf("A: {addr: $%04x, data:$%02x, read:%d} {a: $%02x, x: $%02x, y: $%02x, sp:$%02x, p: $%02x, pc: $%04x}\n",
                systemBusAfter.addressBus, systemBusAfter.dataBus, systemBusAfter.read,
                registersAfter.acc, registersAfter.x, registersAfter.y, registersAfter.stackPointer, registersAfter.statusRegister, registersAfter.programCounter);
        }
    };

    void Cpu2a03::processInstruction() {
        DebugState debugState;

        // Read the next op code from memory (or interrupt)
        OpCode opCode = fetchOpCode();
        debugState.opCode = opCode;
        debugState.registersBefore = *registers;
        debugState.systemBusBefore = *systemBus;

        // Set up system bus to contain relevant memory data for a particular instruction.
        AddressingModeHandler::handleAddressingMode(opCode.addressingMode, *systemBus, *registers, *memoryMapper);
        // Call the instruction handler
        opCode.instructionHandler(opCode, *systemBus, *registers, *memoryMapper);

        // TODO handle return of post-instruction data such as cylce timing and paging 

        // clear interrupt source flag set by hardware pins if any.
        registers->interruptStatus = InterruptLevel::NONE;

        debugState.registersAfter = *registers;
        debugState.systemBusAfter = *systemBus;
        debugState.print();
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
        registers->stackPointer = 0xfd;
        registers->programCounter = 0;  // will do reset vector following

        memset(ram->ram, 0, SystemRam::systemRAMBytes);

    }

    void Cpu2a03::setIrq() {
        registers->interruptStatus = InterruptLevel::IRQ;
    }
    void Cpu2a03::setNmi() {
        registers->interruptStatus = InterruptLevel::NMI;
    }

    // $4011 set to 0
    // reset preserves all registers except pc
    // reset vector is at FFFC,FFFD (usually ROM)
    // https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_note-1
    void Cpu2a03::reset() {

    }
}