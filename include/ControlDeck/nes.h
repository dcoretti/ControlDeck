#pragma once

#include "CPU\cpu2A03.h"
#include "PPU\PPU2C02.h"
#include "cartridge.h"
#include "CPU/SystemComponents.h"

#include "PPU/PPUComponents.h"
#include "Render.h"

namespace NES {
    struct NesControlDeck {
        Cpu2a03 cpu;
        Ppu2C02 ppu;
        Cartridge cart;
    };

    void initNes(char * nesFile, NesControlDeck &nesControlDeck);
    DebugState nesLoop(NesControlDeck &nes, int numInstructions = -1);
    DebugState step(NesControlDeck &nes);
}