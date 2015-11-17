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
        *   PPUCTRL $2000 (Writable)
        *   Bit |    Function
        *   ==================
        *   0   |   Base nametable (0-3) - x table bit 0, y scroll table bit 1 http://nesdev.com/2C02%20technical%20reference.TXT
        *   1   |       (MSBit of X, Y scroll position respectively)
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
        *   PPUMASK $2001  (Writable)
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
        *   PPUSTATUS $2002 (Read)
        *   Bit |   Function
        *   ================
        *   0   |   Least significant bits previously written into a ppu register
        *   1   |
        *   2   |
        *   3   |
        *   4   |
        *   5   |   sprite overflow (more than 8 sprites encountered when checking for objects in Y-range for next scan line
        *   6   |   Sprite 0 hit
        *   7   |   Vertical blank
        *   
        *   Read clears vblank flag on bit 7.
        */
        uint8_t status;

        /**
        *   OAMADDR $2003 (Writable)
        *   Object Attribute Memory. 
        *   Writing to oamData increments oamAddr.
        */
        uint8_t oamAddr;
        // OAMDATA $2004 (Writable)
        uint8_t oamData;

        /**
        *   PPUSCROLL $2005 (Writable)
        *   fine scrolling position set at start of frame
        */
        uint8_t scroll;

        /**
        *   CPU -> VRAM interface via address/data 
        *   Access pattern is to addr (2 instr hsb, lsb) then repeatedly to data.
        */
        // PPUADDR $2006 (Writable)
        uint8_t address;
        // PPUDATA $2007 (Writable)
        uint8_t data;
        // TODO do I need this to represent dummy reads with internal buffer?
        uint8_t dataReadBuffer;

        // OAMDMA $4014 - MSB of 256 byte dma transfer starting location. 
        uint8_t oamDma;
    };

    enum class SpritePriority {
        IN_FRONT_OF_BACKGROUND = 0,
        BEHIND_BACKGROUND
    };

    /**
    *   Single 4byte Object attribute memory details (64 total)
    *   Attributes should always preserve bits 2-4 being zero.
    */
    struct ObjectAttributeMemory {
        // attribute accessors
        void setPalette(uint8_t palette);
        uint8_t getPalette();
        void setPriority(SpritePriority priority);
        SpritePriority getPriority();
        void setHorizontalFlip(bool enabled);
        bool getHorizontalFlip();
        void setVerticalFlip(bool enabled);
        bool getVerticalFlip();

        // byte 0
        uint8_t spriteTopY;
        // byte 1
        uint8_t tileIndex;
        // byte 2 - 
        uint8_t attributes;
        // byte 3
        uint8_t spriteLeftX;
    };

    /**
    *   Background tile context for a given line in the PPU.  
    */
    struct BackgroundLineContext {
        // combined with other registers: vram address, temporary vram address, fine x scroll and first/second write toggle
        
        // Representativeof two 16-bit shift registers representing two background tiles used in a given scan line
        uint8_t nextTile[2];
        uint8_t curTile[2];

        // Palette attributes for 8 pixels in curTile.
        uint8_t paletteAttributes[2];

    };

    /*
    *   Per-frame sprite context
    */
    struct SpriteLineContext {
        // 64 on screen sprites for a given frame
        ObjectAttributeMemory primaryOAM[64];

        // 8 sprites for the current scanline
        ObjectAttributeMemory secondaryOAM[8];
        //bitmap data for 8 sprites (TODO split these out into pairs?)
        uint8_t bitmapData[16];
        // Attribute bytes for the 8 sprites
        uint8_t attributes[8];
        // Per-sprite x-positions
        uint8_t counters[8];
    };
}