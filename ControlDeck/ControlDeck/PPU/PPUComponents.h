#pragma once
#include <cstdint>

namespace NES{

    enum class MasterSlaveSelectMode {
        USE_EXT = 0,    // Palette index for background color comes from EXT pins (0 in NES since they are grounded)
        USE_SLAVE = 1   // Use secondary picture generator to replace background
    };

    enum class IncrementMode {
        ADD_ONE = 0,
        ADD_32 = 1
    };

    enum class SpriteSize {
        SIZE_8_8 = 0,
        SIZE_8_16 = 1
    };


    /**
    *   General PPU Registers mapped by memory to cpu.
    *   Source:  http://wiki.nesdev.com/w/index.php/PPU_programmer_reference
    */
    struct PPURegisters {
        // Control Register Accessors
        uint8_t getNameTable();
        void setNameTable(uint8_t table);

        void setIncrementMode(IncrementMode mode);
        IncrementMode getIncrementMode();

        void setSpritePatternTable(uint8_t table);
        uint8_t getSpritePatternTable();

        void setBackgroundPatternTable(uint8_t table);
        uint8_t getBackgroundPatternTable();

        void setSpriteSize(SpriteSize spriteSize);
        SpriteSize getSpriteSize();

        void setMasterSlaveSelect(MasterSlaveSelectMode mode);
        MasterSlaveSelectMode getMasterSlaveSelect();

        void setGenerateVBlankNmi(bool enabled);
        bool getGenerateVBlankNmi();

        /**
        *   Bit |    Function
        *   ==================
        *   0   |   
        *   1   |   Base nametable (0-3)
        *   2   |   VRAM increment per r/w of ppu data
        *   3   |   Sprite pattern table (8x8 only)
        *   4   |   Background pattern table
        *   5   |   Sprite Size
        *   6   |   PPU master/slave
        *   7   |   Generate NMI on vblank
        *   source: http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#Controller_.28.242000.29_.3E_write
        */
        uint8_t control;

        uint8_t mask;
        uint8_t status;
        uint8_t oamAddr;
        uint8_t oamData;
        uint8_t scroll;
        uint8_t addr;
        uint8_t data;
        uint8_t oamdma;
    };
}