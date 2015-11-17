#include "MemoryMapper.h"
namespace NES {

    void MemoryMapper::doMemoryOperation(SystemBus &systemBus) {
        if (systemBus.addressBus < 0x2000) {
            // 0x0800-0x0FFF  mirror 1
            // 0x1000-0x1FFF  mirror 2
            systemRamHandler(systemBus);
        }
        else if (systemBus.addressBus < 0x4000) {
            //Eight bytes of memory mapped PPU registers mirrored
            ppuRegisterHandler(systemBus);
        }
        // I/O registers
        else if (systemBus.addressBus < 0x4020) {
            if (systemBus.addressBus == OAMDMA) {
                ppuRegisterHandler(systemBus);
            }
        }
        // Expansion ROM
        else if (systemBus.addressBus < 0x6000) {

        }
        // SRAM
        else if (systemBus.addressBus < 0x8000) {

        }
        // up to $10000 is PRG-ROM
        else {

        }
    }

    void MemoryMapper::ppuRegisterHandler(SystemBus &systemBus) {
        // todo handle read-only situations?

        uint16_t actualAddr = 0x2000 + ((systemBus.addressBus - 0x2000) % 8);
        uint8_t *reg = nullptr;;
        switch (actualAddr) {
        case PPUCTRL:
            reg = &ppuRegisters->control;
            break;
        case PPUMASK:
            reg = &ppuRegisters->mask;
            break;
        case PPUSTATUS:
            reg = &ppuRegisters->status;
            break;
        case OAMADDR:
            reg = &ppuRegisters->oamAddr;
            break;
        case OAMDATA:
            reg = &ppuRegisters->oamData;
            break;
        case PPUSCROLL:
            reg = &ppuRegisters->scroll;
            break;
        case PPUADDR:
            reg = &ppuRegisters->address;
            break;
        case PPUDATA:
            reg = &ppuRegisters->data;
            break;
        case OAMDMA:
            reg = &ppuRegisters->oamDma;
            break;
        default:
            break;
        }

        if (reg != nullptr) {
            if (systemBus.read) {
                systemBus.dataBus = *reg;
            } else {
                *reg = systemBus.dataBus;
            }
        }
    }

    // Read and write main CPU memory.  dataBusBuffer holds intermediate read value to be read
    // by instruction into register etc.
    void MemoryMapper::systemRamHandler(SystemBus &systemBus) {
        size_t actual = systemBus.addressBus % 0x800;
        if (systemBus.read) {
            systemBus.dataBus = systemRam->ram[actual];
        } else {
            systemRam->ram[actual] = systemBus.dataBus;
        }
    }

}