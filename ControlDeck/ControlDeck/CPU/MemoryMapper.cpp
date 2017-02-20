#include "MemoryMapper.h"
namespace NES {
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
    unsigned int MemoryMapper::doMemoryOperation(SystemBus &systemBus) {
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
                dmaData->activate(systemBus.dataBus);
            }
        }
        // General cartrige space including PRG ROM/RAM, SRAM/WRAM (save data), mapper registers, etc.
        else {
            cartridge->mmc->doMemoryOperation(systemBus, *cartridge);
        }

        // TODO memory cycles cost by memory op? 
        return 0;
    }

    void MemoryMapper::ioRegisterHandler(SystemBus &systemBus) {
        DBG_ASSERT(systemBus.addressBus < 0x4020 && systemBus.addressBus > 0x4000, "invalid address for io handler %d", systemBus.addressBus);

    }

    void MemoryMapper::ppuRegisterHandler(SystemBus &systemBus) {
        uint16_t actualAddr = 0x2000 + ((systemBus.addressBus - 0x2000) % 8);

        PPURegister reg = (PPURegister)(actualAddr - 0x2000);
        if (systemBus.read) {
            ppu->readRegister(reg);
        } else {
            ppu->writeRegister(reg, systemBus.dataBus);
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