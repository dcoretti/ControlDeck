#include "AddressingModeHandler.h"

namespace NES {
    void AddressingModeHandler::handleAddressingMode(const AddressingMode addressingMode, SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        switch (addressingMode) {
        case AddressingMode::Absolute:
            getAbsoluateAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::AbsoluteX:
            getXIndexedAbsoluteAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::AbsoluteY:
            getYIndexedAbsoluteAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::Accumulator:
            // no memory access used
            return;
        case AddressingMode::Immediate:
            getImmediateAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::Implicit:
            // no memory access used
			return;
        case AddressingMode::Indirect:
            getIndirectAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::IndirectYIndexed:
            getIndirectYIndexedAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::Relative:
            getRelativeAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::XIndexedIndirect:
            getXIndexedIndirectAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::ZeroPage:
            getZeroPageAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::ZeroPageX:
            getXIndexedZeroPageAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::ZeroPageY:
            getYIndexedZeroPageAddress(systemBus, registers, memoryMapper);
            break;
        case AddressingMode::Undefined:
        default:
			DBG_CRASH("Invalid address mode encountered for instruction %d", addressingMode);
            return;
        }

        // Do final read
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);

        // Fix program counter
        registers.programCounter += addressingModeProgramCounterDelta[addressingMode];
    }

    void readFromAddress(SystemBus &systemBus, MemoryMapper &memoryMapper, uint16_t addr) {
        systemBus.addressBus = addr;
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }


    /**
    *    Operate on data directly held in instruction operand
    *    1 Cycle:
    *        1. Get data from PC+1
    */
    void AddressingModeHandler::getImmediateAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        systemBus.addressBus = registers.programCounter;
        systemBus.read = true;
    }

    /**
    *    Branch specific instructions need to fetch the next instruction.
    *    1 Cycle:
    *        1. Fetch operand for branch jump to be used when determining conditional
    */
    void AddressingModeHandler::getRelativeAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        systemBus.addressBus = registers.programCounter;
        systemBus.read = true;
    }

    /**
    *    Get address in zero page ($0000-$00ff)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)        
    */ 
    void AddressingModeHandler::getZeroPageAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        // get operand addr
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        // setup from zero page
        systemBus.setAdlOnly(systemBus.dataBus);
    }

    /**
    *    Get address in zero page ($0000-$00ff) + X (with wraparound in zero page!)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void AddressingModeHandler::getXIndexedZeroPageAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        systemBus.setAdlOnly((systemBus.dataBus + registers.x) % 0x80);
    }

    /**
    *    Get address in zero page ($0000-$00ff) + Y (with wraparound in zero page!)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void AddressingModeHandler::getYIndexedZeroPageAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        systemBus.setAdlOnly((systemBus.dataBus + registers.y) % 0x80);
    }

    /**
    *    2 Cycles:
    *        1. Fetch ADL
    *        2. Fetch ADH
    */
    void AddressingModeHandler::getAbsoluateAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        uint8_t adlTmp = systemBus.dataBus;
        readFromAddress(systemBus, memoryMapper, registers.programCounter+1);
        systemBus.setAddressBus(adlTmp, systemBus.dataBus);
    }

    void AddressingModeHandler::getXIndexedAbsoluteAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        getAbsoluateAddress(systemBus, registers, memoryMapper);
        systemBus.addressBus += registers.x;
    }

    void AddressingModeHandler::getYIndexedAbsoluteAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        getAbsoluateAddress(systemBus, registers, memoryMapper);
        systemBus.addressBus += registers.y;
    }


    /**
    *    JMP specific addressing mode: load the address located at the memory location specified by the operands
    *    4 Cycles:
    *        1. Fetch ADL (indirect address)
    *        2. Fetch ADH (indirect address)
    *        3. Fetch Jump L address from indirect address in memory
    *        4. Fetch Jump H address
    */
    void AddressingModeHandler::getIndirectAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        getAbsoluateAddress(systemBus, registers, memoryMapper);

        fetchIndirectAddressToBus(systemBus, memoryMapper);
    }

    /**
    *    Also called Pre-indexed.  X + operand gives the address of the location of the data
    *    3 Cycles:
    *        1. Fetch ADL (indirect address)
    *        2. Fetch [ADL]+X (location of final address)
    *        3. Fetch ADL+X+1
    */
    void AddressingModeHandler::getXIndexedIndirectAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        systemBus.setAdlOnly(systemBus.dataBus + registers.x);
        fetchIndirectAddressToBus(systemBus, memoryMapper);
        // address bus now contains the address retrieved from x in zero page.
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
    void AddressingModeHandler::getIndirectYIndexedAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        systemBus.setAdlOnly(systemBus.dataBus);
        fetchIndirectAddressToBus(systemBus, memoryMapper);

        systemBus.addressBus += registers.y;
    }

    /**
    * Helper for indirect addressing: given a system bus with the location of the indirect address, fetch the 16bit address starting at that location
    * and set it on the systemBus addressBus.
    */
    void AddressingModeHandler::fetchIndirectAddressToBus(SystemBus& systemBus, MemoryMapper& memoryMapper) {
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
        uint8_t tmpAdl = systemBus.dataBus;
        systemBus.addressBus++; // adh at next word
        memoryMapper.doMemoryOperation(systemBus);

        systemBus.setAddressBus(tmpAdl, systemBus.dataBus);
    }

}