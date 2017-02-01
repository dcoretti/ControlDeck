#pragma once
#include <cstdint>
#include "PPUComponents.h"
#include "../cartridge.h"

namespace NES {

    class PPUMemoryMap {
    public:
        static const uint16_t patternTableBoundary = 0x4000;
        static const uint16_t nameTableBoundary = 0x3f00;

        uint8_t getByte(uint16_t address);

        Cartridge *cartridge;
    private:
        PatternTable * patternTables[2];
        AttributeTable * attributeTables[4];
        NameTable * nameTables[4];
        SystemColorPalette * colorPalettes;
    };

}