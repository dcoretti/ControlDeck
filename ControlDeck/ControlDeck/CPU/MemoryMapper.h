#pragma once
#include "SystemComponents.h"
#include "../PPU/PPU2C02.h"
#include "../common.h"
#include "../cartridge.h"
namespace NES {
    /**
    *   Map memory from the CPU address space, to RAM, PPU, APU, and cartridge components.
    **/
    class MemoryMapper {
    public:
        MemoryMapper() = default;
        MemoryMapper(SystemRam * systemRam, Ppu2C02 * ppu = nullptr) : systemRam(systemRam), ppu(ppu) {}

        /**
        * returns number of cycles the memory operation costs.
        * This allows functions such as DMA to be handled in one operation that the CPU simply waits out.
        */
        unsigned int doMemoryOperation(SystemBus &systemBus);

        Cartridge *cartridge;

        Ppu2C02 *ppu{ nullptr };
        SystemRam * systemRam{ nullptr };
        DMAData *dmaData{ nullptr };
    private:
        void systemRamHandler(SystemBus &systemBus);
        void ppuRegisterHandler(SystemBus &systemBus);
        void ioRegisterHandler(SystemBus &systemBus);

        //MmcHandler mmcHandler;    // active MMC memory access handler
    };

}