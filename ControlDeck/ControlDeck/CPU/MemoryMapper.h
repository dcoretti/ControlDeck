#pragma once
#include "SystemComponents.h"
#include "../PPU/PPUComponents.h"
#include "../common.h"
#include "../cartridge.h"
namespace NES {
    enum PPURegisterMemoryMapping {
        PPUCTRL = 0x2000,
        PPUMASK = 0x2001,
        PPUSTATUS = 0x2002,
        OAMADDR = 0x2003,
        OAMDATA = 0x2004,
        PPUSCROLL = 0x2005,
        PPUADDR = 0x2006,
        PPUDATA = 0x2007,
        OAMDMA = 0x4014
    };


    /**
    *   Map memory from the CPU address space, to RAM, PPU, APU, and cartridge components.
    **/
    class MemoryMapper {
    public:
        MemoryMapper() = default;
        MemoryMapper(SystemRam * systemRam, PPURegisters * ppuRegisters = nullptr) : systemRam(systemRam), ppuRegisters(ppuRegisters) {}

        /**
        * returns number of cycles the memory operation costs.
        * This allows functions such as DMA to be handled in one operation that the CPU simply waits out.
        */
        unsigned int doMemoryOperation(SystemBus &systemBus);

        Cartridge *cartridge;

        PPURegisters * ppuRegisters{ nullptr };     // TODO remove this in favor of PPU memory mapper handling everything...
        SystemRam * systemRam{ nullptr };
    private:
        void systemRamHandler(SystemBus &systemBus);
        void ppuRegisterHandler(SystemBus &systemBus);
        void ioRegisterHandler(SystemBus &systemBus);

        //MmcHandler mmcHandler;    // active MMC memory access handler
    };

}