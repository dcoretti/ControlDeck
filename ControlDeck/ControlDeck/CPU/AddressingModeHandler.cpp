#include "AddressingModeHandler.h"


namespace NES {
    AddressingModeHandler::OpCodeArgs AddressingModeHandler::handleAddressingMode(const AddressingMode addressingMode, SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper) {
        AddressingModeHandler::OpCodeArgs opCodeArgs = AddressingModeHandler::OpCodeArgs();
        switch (addressingMode) {
        case AddressingMode::Absolute:
            getAbsoluateAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::AbsoluteX:
            getXIndexedAbsoluteAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::AbsoluteY:
            getYIndexedAbsoluteAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::Accumulator:
            // no memory access used
            return opCodeArgs;
        case AddressingMode::Immediate:
            getImmediateAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::Implicit:
            // no memory access used
            return opCodeArgs;
        case AddressingMode::Indirect:
            getIndirectAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::IndirectYIndexed:
            getIndirectYIndexedAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::Relative:
            getRelativeAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::XIndexedIndirect:
            getXIndexedIndirectAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::ZeroPage:
            getZeroPageAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::ZeroPageX:
            getXIndexedZeroPageAddress(systemBus, registers, memoryMapper, opCodeArgs);
            break;
        case AddressingMode::ZeroPageY:
            getYIndexedZeroPageAddress(systemBus, registers, memoryMapper, opCodeArgs);
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
    void AddressingModeHandler::getImmediateAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        systemBus.addressBus = registers.programCounter;
        systemBus.read = true;

        memoryMapper.doMemoryOperation(systemBus);
        args.args[0] = systemBus.dataBus;
    }

    /**
    *    Branch specific instructions need to fetch the next instruction.
    *    1 Cycle:
    *        1. Fetch operand for branch jump to be used when determining conditional
    */
    void AddressingModeHandler::getRelativeAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        systemBus.addressBus = registers.programCounter;
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
        args.args[0] = systemBus.dataBus;
    }

    /**
    *    Get address in zero page ($0000-$00ff)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)        
    */ 
    void AddressingModeHandler::getZeroPageAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        // get operand addr
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        args.args[0] = systemBus.dataBus;

        // setup from zero page
        systemBus.setAdlOnly(systemBus.dataBus);

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }

    /**
    *    Get address in zero page ($0000-$00ff) + X (with wraparound in zero page!)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void AddressingModeHandler::getXIndexedZeroPageAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        args.args[0] = systemBus.dataBus;

        systemBus.setAdlOnly((systemBus.dataBus + registers.x) % 0x80);

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }

    /**
    *    Get address in zero page ($0000-$00ff) + Y (with wraparound in zero page!)
    *    1 Cycle:
    *        1. fetch addr operand (1 byte)
    */
    void AddressingModeHandler::getYIndexedZeroPageAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        args.args[0] = systemBus.dataBus;

        systemBus.setAdlOnly((systemBus.dataBus + registers.y) % 0x80);

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }

    /**
    *    2 Cycles:
    *        1. Fetch ADL
    *        2. Fetch ADH
    */
    void AddressingModeHandler::fetchAddressFromPCToBus(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        uint8_t adlTmp = systemBus.dataBus;
        readFromAddress(systemBus, memoryMapper, registers.programCounter + 1);
        systemBus.setAddressBus(adlTmp, systemBus.dataBus);

        args.args[0] = adlTmp;
        args.args[1] = systemBus.dataBus;
    }

    void AddressingModeHandler::getAbsoluateAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        fetchAddressFromPCToBus(systemBus, registers, memoryMapper, args);
        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }

    void AddressingModeHandler::getXIndexedAbsoluteAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        fetchAddressFromPCToBus(systemBus, registers, memoryMapper, args);
        if ((systemBus.addressBus & 0xff) + registers.x >= 0xff) {
            // page boundary crossed
            args.pagingInstructions = 1;
        }
        systemBus.addressBus += registers.x;

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }

    void AddressingModeHandler::getYIndexedAbsoluteAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        fetchAddressFromPCToBus(systemBus, registers, memoryMapper, args);
        if ((systemBus.addressBus & 0xff) + registers.y >= 0xff) {
            // page boundary crossed
            args.pagingInstructions = 1;
        }
        systemBus.addressBus += registers.y;

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }


    /**
    *    JMP specific addressing mode: load the address located at the memory location specified by the operands
    *    4 Cycles:
    *        1. Fetch ADL (indirect address)
    *        2. Fetch ADH (indirect address)
    *        3. Fetch Jump L address from indirect address in memory
    *        4. Fetch Jump H address
    */
    void AddressingModeHandler::getIndirectAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        fetchAddressFromPCToBus(systemBus, registers, memoryMapper, args);

        fetchIndirectAddressToBus(systemBus, memoryMapper);

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
    }

    /**
    *    Also called Pre-indexed.  X + operand gives the address of the location of the data
    *    3 Cycles:
    *        1. Fetch ADL (indirect address)
    *        2. Fetch [ADL]+X (location of final address)
    *        3. Fetch ADL+X+1
    */
    void AddressingModeHandler::getXIndexedIndirectAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        args.args[0] = systemBus.dataBus;

        systemBus.setAdlOnly(systemBus.dataBus + registers.x);
        fetchIndirectAddressToBus(systemBus, memoryMapper);
        // address bus now contains the address retrieved from x in zero page.

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
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
    void AddressingModeHandler::getIndirectYIndexedAddress(SystemBus &systemBus, Registers &registers, MemoryMapper &memoryMapper, OpCodeArgs &args) {
        readFromAddress(systemBus, memoryMapper, registers.programCounter);
        args.args[0] = systemBus.dataBus;

        systemBus.setAdlOnly(systemBus.dataBus);
        fetchIndirectAddressToBus(systemBus, memoryMapper);
        if ((systemBus.addressBus & 0xff) + registers.y > 0xff) {
            args.pagingInstructions = 1;
        }
        systemBus.addressBus += registers.y;

        systemBus.read = true;
        memoryMapper.doMemoryOperation(systemBus);
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