#pragma once

#include "CPU\cpu2A03.h"
#include "PPU\PPU2C02.h"
#include "cartridge.h"
#include "CPU/MemoryMapper.h"
#include "CPU/SystemComponents.h"

#include "PPU/PPUComponents.h"


// APU
namespace NES {
    struct NesControlDeck {
        Cpu2a03 cpu;
        Ppu2C02 ppu;
        Cartridge cart;

        MemoryMapper memoryMapper;
        SystemRam systemRam{};
        SystemBus systemBus{};
        Registers registers{};
    };


    void initNes(char * nesFile, NesControlDeck &nesControlDeck);
    void nesLoop(NesControlDeck &nes, int numInstructions = -1);

}