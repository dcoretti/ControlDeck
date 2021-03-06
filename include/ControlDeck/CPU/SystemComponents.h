#pragma once

#include <cstdint>

namespace NES {
    // Bit location of processor status flags 
    enum ProcessorStatus {
        CarryFlag = 0,            // Last operation caused overflow from bit 7.  
        ZeroFlag = 1,            // Result of last operation was 0
        InterruptDisable = 2,    // While set processor will not respond to interrupts from devices until cleared via CLI
        DecimalMode = 3,        // Not used (make arithmetic use BCD)
        // Bits 4 and 5 are never set on the physical status register but instead are set as part of pushing the status register to the stack
        // to identify interrupt sources
        // see: http://wiki.nesdev.com/w/index.php/CPU_ALL#The_B_flag
        BreakCommand = 4,        // Set after BRK and interrupt set to process
        //Interrupt = 5,
        OverflowFlag = 6,        // Look at carry between bit 6 and 7, 7 and carry flag to determine if result switched sign via overflow
        NegativeFlag = 7        // If bit 7 set to 1 as a result of last operation
    };

    enum InterruptType {
        INT_NONE,
        INT_IRQ,
        INT_NMI,
        INT_RESET,
        INT_BRK
    };

    // Sources:  http://nesdev.com/NESDoc.pdf
    struct Registers {
        // Reference: http://e-tradition.net/bytes/6502/6502_instruction_set.html for flags altered by a given instruction
        // Program counter low byte
        inline uint8_t  pcl() {
            return (uint8_t)(programCounter & 0xff);
        }

        // Program counter high byte
        inline uint8_t pch() {
            return (uint8_t)(programCounter >> 8);
        }

        inline void setPcl(uint8_t l) {
            programCounter = (programCounter & 0xff00) + l;
        }

        inline void setPch(uint8_t h) {
            programCounter = (programCounter & 0xff) | (h << 8);
        }

        // Status register utils
        // program counter low byte
        inline bool flagSet(ProcessorStatus flag) {
            return (statusRegister & (1 << flag)) != 0;
        }

        inline void setFlag(ProcessorStatus flag) {
            statusRegister |= (1 << flag);
        }

        inline void clearFlag(ProcessorStatus flag) {
            statusRegister &= ~(1 << flag);
        }

        inline void setFlagIfNegative(uint8_t val) {
            if ((val & 0x80) != 0) {
                setFlag(ProcessorStatus::NegativeFlag);
            } else {
                clearFlag(ProcessorStatus::NegativeFlag);
            }
        }

        inline void setFlagIfZero(uint8_t val) {
            if (val == 0) {
                setFlag(ProcessorStatus::ZeroFlag);
            } else {
                clearFlag(ProcessorStatus::ZeroFlag);
            }
        }

        inline bool willAddOverflow(uint8_t val) {
            // acc ^ val will have a 1 in sign bit if signs are different between them
            // See if the sign changed from acc to the result and check if the sign bit is set between them
            return (((~(acc ^ val))&(acc ^ (acc+val))) & 0x80) > 0;
        }

        inline bool willSubOverflow(uint8_t val) {
            return (int16_t)acc - (int16_t)val <= -128 || (int16_t)acc - (int16_t)val > 127;
        }

        // will the carry flag be set or cleared on an addition with the accumulator
        // see http://www.6502.org/tutorials/vflag.html
        inline bool willAddCarry(uint8_t val) {
            return acc + val > 0xff;
        }
        
        // will the carry flag be set or cleared for a subtraction from the accumulator
        inline bool willSubCarry(uint8_t val) {
            return acc - val <= 0;
        }

        void statusToString(char out[8]) {
            // Rearranged s/t bit 0(carry) is out[7] to read l to r
            out[0] = 'n' - (((statusRegister & 0x80) > 0) * 32);
            out[1] = 'v' - (((statusRegister & 0x40) > 0) * 32);
            out[2] = 'u' - (((statusRegister & 0x20) > 0) * 32);
            out[3] = 'b' - (((statusRegister & 0x10) > 0) * 32);
            out[4] = 'd' - (((statusRegister & 0x8) > 0) * 32);
            out[5] = 'i' - (((statusRegister & 0x4) > 0) * 32);
            out[6] = 'z' - (((statusRegister & 0x2) > 0) * 32);
            out[7] = 'c' - (((statusRegister & 0x1) > 0) * 32);
            out[9] = '\0';
        }

        // Commonly used for counters/offsets in memory accesss.  
        // Can be loaded/saved in memory, compared with memory values.
        // Can be used to get a copy of the stack pointer/change its value
        uint8_t x;

        // Commonly used for counters/offsets in memory accesss.  
        // Same as x but no ability to alter stack
        uint8_t y;

        // can be stored/retrieved from memory or stack (Commonly referred to as the A register)
        uint8_t acc;

        // Register holds low 8 bits of the next free location on the stack
        // Stack located between $0100 and $01FF. 
        uint8_t stackPointer;

        // Program status flag register  (Commonly referred to as P)
        uint8_t statusRegister;

        uint16_t programCounter;

        // interrupts needing to be processed.
        InterruptType interruptStatus;
    };

    /**
    *    System bus represents the state of the 16bit address lines and 8 bit data bus lines connecting the CPU to RAM or mapped memory.
    *   Each cycle involves a memory operation (read or a write).  The general usage is to set the address/data bus state depending on read/write
    *   and tell the memory mapper to handle the bus state.
    *   Read operations overwrite dataBus with the value in mapped memory for retrieval.
    *   See also http://users.telenet.be/kim1-6502/6502/hwman.html Appendix A for read/write ops, 1.3.1 for general concepts
    */
    struct SystemBus {
        inline void setAdh(uint8_t val) {
            // clear high order bits
            addressBus &= 0x00ff;
            addressBus |= (val << 8);
        }

        inline void setAdl(uint8_t val) {
            uint16_t mask = 0;
            mask |= (uint16_t)val;
            addressBus |= mask;
        }

        inline void setAdhOnly(uint8_t val) {
            addressBus = val << 8;
        }

        inline void setAdlOnly(uint8_t val) {
            addressBus = val;
        }

        inline void setAddressBus(uint8_t adl, uint8_t adh) {
            addressBus = (adh << 8) + adl;
        }

        uint16_t addressBus{ 0 };
        uint8_t dataBus{ 0 };
        bool read{ false };
    };

    struct SystemRam {
        // Actual System RAM
        static const size_t systemRAMBytes = 2048;
        uint8_t ram[systemRAMBytes]{ 0 };
    };

    struct DMAData {
        void activate(uint8_t baseDmaAddress) {
            baseAddress = baseDmaAddress;
            isActive = true;
            cycleCounter = 0;
            bytesWritten = 0;
            curByteToWrite = 0;
        }
        
        bool isActive{ false };
        // OAMDMA $4014 - MSB of 256 byte dma transfer starting location. 
        uint8_t baseAddress{ 0 };
        uint16_t cycleCounter{ 0 };
        int bytesWritten{ 0 };
        uint8_t curByteToWrite{ 0 };
    };
}