#pragma once
#include <cstdint>

namespace NES{

    // Control types
    
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

    // Mask types
    enum ColorEmphasis {
        EMPHASIZE_RED_NTSC = 0x20,
        EMPHASIZE_GREEN_PAL = 0x20,
        EMPHASIZE_GREEN_NTSC = 0x40,
        EMPHASIZE_RED_PAL = 0x40,
        EMPHASIZE_BLUE_NTSC = 0x80,
        EMPHASIZE_BLUE_PAL = 0x80
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


        // Mask register accessors
        void setGrayscale(bool enabled);
        bool getGrayscale();

        void setShowBackgroundLeft(bool enabled);
        bool getShowBackgroundLeft();

        void setShowSpritesLeft(bool enabled);
        bool getShowSpritesLeft();

        void setShowBackground(bool enabled);
        bool getShowBackground();

        void setShowSprites(bool enabled);
        bool getShowSprites();

        void setColorEmphasis(ColorEmphasis emphasis);
        bool getColorEmphasis(ColorEmphasis emphasis);

        // Status register
        void setSpriteOverflow(bool overflow);
        bool getSpriteOverflow();

        void setSpriteZeroHit(bool hit);
        bool getSpriteZeroHit();

        bool getVBlank();
        void setVBlank(bool vblank);

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

        /**
        *   Bit |   Function
        *   ================
        *   0   |   Grayscale
        *   1   |   Show background for leftmost 8pixels (scrolling)
        *   2   |   Show sprites in  leftmost 8 pixels
        *   3   |   Show background
        *   4   |   Show sprites
        *   5   |   Emphasize red (ntsc)
        *   6   |   Emphasize green (ntsc)
        *   7   |   Emphasize blue (ntsc)
        */
        uint8_t mask;

        /**
        *   Bit |   Function
        *   ================
        *   0   |   Least significant bits previously written into a ppu register
        *   1   |
        *   2   |
        *   3   |
        *   4   |
        *   5   |   sprite overflow
        *   6   |   Sprite 0 hit
        *   7   |   Vertical blank
        */
        uint8_t status;

        /**
        *   Writing to oamData increments oamAddr.
        */
        uint8_t oamAddr;
        uint8_t oamData;

        // scrolling position
        uint8_t scroll;


        uint8_t addr;
        uint8_t data;

        // MSB of 256 byte dma transfer starting location. 
        uint8_t oamDma;
    };
}