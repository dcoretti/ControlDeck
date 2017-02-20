#pragma once
#include <cstdint>
#include "PPUComponents.h"
#include "../cartridge.h"

namespace NES {
    class PPUMemoryMap {
    public:
        static const uint16_t patternTableBoundary = 0x4000;
        static const uint16_t nameTableBoundary = 0x3f00;

        uint8_t doMemoryOperation(uint16_t address, uint8_t write, bool read = true);
        uint8_t getByte(uint16_t address) { return doMemoryOperation(address, 0); }
        bool isAddressInPaletteRange(uint16_t address);
        Cartridge *cartridge;
        PPUMemoryComponents *ppuComponents;
    };
}