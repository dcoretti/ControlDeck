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

		PPURegisters * ppuRegisters{ nullptr };
		SystemRam * systemRam{ nullptr };
    private:
        void systemRamHandler(SystemBus &systemBus);
        void ppuRegisterHandler(SystemBus &systemBus);
        void ioRegisterHandler(SystemBus &systemBus);

        //MmcHandler mmcHandler;    // active MMC memory access handler
    };

}