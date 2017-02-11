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
    *   General exposed PPU Registers mapped by memory to cpu.
    *   Source:  http://wiki.nesdev.com/w/index.php/PPU_programmer_reference
    */
    struct PPURegisters {
        // Control Register Accessors
        uint16_t getNameTableBaseAddr();
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

        bool isRenderingEnabled();

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
        *   3   |   Which sprite pattern table (8x8 only)
        *   4   |   Background pattern table
        *   5   |   Sprite Size
        *   6   |   PPU master/slave
        *   7   |   Generate NMI on vblank (Enabling this flag will output NMI to CPU when STATUS vblank flag is also set)
        *   source: http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#Controller_.28.242000.29_.3E_write
        */
        uint8_t control{ 0 };

        /**
        *   PPUMASK $2001  (Writable)
        *   Bit |   Function
        *   ================
        *   0   |   Grayscale
        *   1   |   Show background for leftmost 8pixels (scrolling)
        *   2   |   Show sprites in  leftmost 8 pixels
        *   3   |   Show background (enabling this turns on rendering)
        *   4   |   Show sprites    (enabling this turnes on rendering)
        *   5   |   Emphasize red (ntsc)
        *   6   |   Emphasize green (ntsc)
        *   7   |   Emphasize blue (ntsc)
        */
        uint8_t mask{ 0 };

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
        *   7   |   Vertical blank (Used for generating NMI when CTRL bit 7 is also set)
        *   
        *   Read clears vblank flag on bit 7.
        */
        uint8_t status{ 0 };

        /**
        *   OAMADDR $2003 (Writable)
        *   Object Attribute Memory. 
        *   Writing to oamData increments oamAddr.
        */
        uint8_t oamAddr{ 0 };
        // OAMDATA $2004 (Writable)
        uint8_t oamData{ 0 };

        /**
        *   PPUSCROLL $2005 (Writable)
        *   Scrolling position set at start of next frame (pulled into rendering context registers)
        *   used for general scrolling capabilities by modifying over frames as new information is added to nametables
        *   see: http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#PPUSCROLL 
        *   written to twice, X followed by Y to introduce x and y scrolling.
        *
        *   Scrolling split into two halves Fine scrolling (3  LSBits) and coarse, (upper 5 bits) during rendering.
        *   See http://wiki.nesdev.com/w/index.php/PPU_scrolling summary diagram for how splitting ocrurs on writes 
        *   to this register.
        *
        *   TODO handle upper values (240-255) properly to reflect -16-1? see ref
        */
        uint8_t scroll{ 0 };

        /**
        *   CPU -> VRAM interface via address/data 
        *   Access pattern is to addr (2 instr hsb, lsb) then repeatedly to data.
        */
        // PPUADDR $2006 (Writable)
        uint8_t address{ 0 };
        // PPUDATA $2007 (Writable)
        uint8_t data{ 0 };
        // TODO do I need this to represent dummy reads with internal buffer?
        uint8_t dataReadBuffer{ 0 };

        // OAMDMA $4014 - MSB of 256 byte dma transfer starting location. 
        uint8_t oamDma{ 0 };
    };

    enum class SpritePriority {
        IN_FRONT_OF_BACKGROUND = 0,
        BEHIND_BACKGROUND
    };

    /**
    *   Single 4byte Object attribute memory details (64 total)
    *   Attributes should always preserve bits 2-4 being zero.
    *   Also known as SPR-RAM or 
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

        uint8_t getTileIndex(bool is8x16);
        uint8_t getPatternTableFor8x16();

        // byte 0
        uint8_t spriteTopY{ 0 };
        // byte 1
        // (8x8 sprites): tile number of the sprite in the pattern table selected 
        // (8x16 sprites):pattern table is selected from bit0 
        uint8_t tileIndex{ 0 };
        /**
        *   byte 2 - Attributes
        *   Bit |    Function
        *   ==================
        *   0   |   Palette number
        *   1   |   Palette number
        *   2   |   
        *   3   |   Not Used
        *   4   | 
        *   5   |   Priority 0: in front of background, 1: behind
        *   6   |   Horizontal flip
        *   7   |   Vertical flip
        *  
        */
        uint8_t attributes{ 0 };
        // byte 3
        uint8_t spriteLeftX{ 0 };
    };

    /**
    *   Each color is an 8 bit index into the global color palette (64 color entries).
    *   Each palette is a set of 3 color indices.
    *   4th entry is unused by rendering but is writeable (see memory map)
    *   http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#Palettes
    */
    struct ColorPalette {
        uint8_t colorIndex[3]{};
    };

    /**
    *   Available colors for rendering. 
    *   Attribute table entry determines which palette (most significant bits)
    *   Pattern table (l and r half) specify which color in the palette
    */
    struct SystemColorPalette {
        // vram $3f00 (mirrored every 4 bytes until 3f1c)
        uint8_t universalBackgroundColor{};
        // vram $3f01-$3f0f
        ColorPalette backgroundPalettes[4]{};
        //vram $3f11 - $3f1f
        ColorPalette spritePalette[4]{};

        // $3f04 $3f08 $3f0c mirror $3f14 $3f18 $3f1c
        // not used in rendering normally
        uint8_t unusedPaletteData[3]{};
    };

    /**
    *   8x8 pixel tile to be drawn on the screen.  Each half represents one bit of the palette table entry
    *   corresponding to the color to be displayed on the screen.
    */
    struct PatternTableEntry {
        uint8_t colorPlaneBit0[8]{};
        uint8_t colorPlaneBit1[8]{};
    };

    /** 
    *   PPU Pattern table of 8x8 pixel tiles (one at both $0000 and $1000)
    *   Used for both background and sprite pixel data
    *   see: http://wiki.nesdev.com/w/index.php/PPU_pattern_tables
    */
    struct PatternTable {
        PatternTableEntry patterns[256]{};
    };

    /**
    *  Background data attribute table
    *   Each tileGroup byte represents a 4x4 set of tiles by containing the upper 2 bits of each tile's color
    *   The screen is divided into 32x32 pixel tiles (a 4x4 group of 8px x 8px name table tiles) represented by a byte each.
    *   Each 32x32 tile has four 16x16 sections represented by 2 bits of that byte
    *   Each 16x16 area
    */
    struct AttributeTable {
        uint8_t getTileBitsFromGroup(uint8_t group, uint8_t tile);
        uint8_t tileGroup[64]{};
    };

    /**
    *   Background Name table is a matrix which holds index values to pattern table entries 
    *   (1 byte per name table entry means 256 pattern table entries can be indexed)
    *   Each name table entry represents an 8x8 set of pixels for a total of 32x30 tiles or 256x240 pixels.
    */
    struct NameTable {
        // 32 tile wide (* 8 = 256) 30 tiles height (* 8 = 240) for 256x240 screen.
       
//        uint8_t tileIndices[32][30];
        uint8_t nameTable[32 * 30]{};
        AttributeTable attributeTable{};
    };



    // Context structures representing the current memory being used in a given scan line

    /**
    *   Background tile context for a given line in the PPU.
    */
    struct BackgroundTileMemory {
        // combined with other registers: vram address, temporary vram address, fine x scroll and first/second write toggle

        // Representative of two 16-bit shift registers representing two background tiles used in a given scan line
        // 
        uint16_t bitmapTileData[2]{};

        // Palette attributes for 8 pixels in curTile.
        uint8_t paletteAttributes[2]{};

        void setNexTileBitmapData(size_t registerNum, uint8_t data) {
            bitmapTileData[registerNum] &= 0x00ff;
            bitmapTileData[registerNum] |= (data << 8);
        }

        uint8_t getPixelToRender(size_t registerNum) {
            return (uint8_t)bitmapTileData[registerNum] & 0x00ff;
        }
    };

    /*
    *   Per-frame sprite context
    *  https://wiki.nesdev.com/w/index.php/PPU_rendering#Preface
    */
    struct ScanLineBitMapDataShiftRegisters {
        uint8_t bitmapData[2]{};
    };

    const uint8_t spritesPerScanLine = 8;
    const uint8_t spritesPerFrame = 64;
    struct SpriteMemory {
        // 64 on screen sprites for a given frame
        ObjectAttributeMemory primaryOAM[spritesPerFrame]{};

        // 8 sprites for the current scanline
        ObjectAttributeMemory secondaryOAM[spritesPerScanLine]{};
        //bitmap data for 8 sprites (in pairs of shift registers)
        ScanLineBitMapDataShiftRegisters bitmapData[spritesPerScanLine]{};
        // Attribute bytes for the 8 sprites
        uint8_t attributes[spritesPerScanLine]{};
        // Per-sprite x-positions
        uint8_t counters[spritesPerScanLine]{};
    };


    /**
    *   Registers used during a render loop for ouput pixel determination.  vramAddress composed to 
    *   be used in data register to access vram.
    *
    *   ref: http://wiki.nesdev.com/w/index.php/PPU_scrolling
    *
    * TODO combine with backgroundTileMemory?
    */
    struct PPURenderingRegisters {
        // TODO consider dropping bit fields due to initialization
        PPURenderingRegisters() {
            vramAddress = 0;
            tempVramAddress = 0;
            fineXScroll = 0;
        }

        // Use "firstWrite" to determine if x or y is being written to.    
        //??
        void setVramAddress(uint8_t fineYScroll, uint8_t nametableSelect, uint8_t coarseY, uint8_t coarseX);
        
        // Callbacks to handle linked behavior between CPU registers and rendering registers AFTER registers are updated
        void onControlWrite(PPURegisters &registers);
        void onScrollWrite(PPURegisters &registers);
        void onAddressWrite(PPURegisters &registers);
        void onDataAccess(PPURegisters &registers);
        /**
        *   Vram address and temp address composition
        *   Bit   |    Function
        *   ======================
        *   0-4   |  coarse x scroll (upper X bits)  
        *   5-9   |  coarse 9 scroll (upper Y bits)
        *   10-11 |  nametable select
        *   12-14 |  fine Y scroll
        *   
        */
        uint16_t vramAddress: 15;
        uint16_t tempVramAddress: 15;

        // Fine x scroll set via scroll register
        uint8_t fineXScroll : 3;
        
        //Toggle to determine x vs y status update, __
        // reset of this is latched to status register read 
        // False (0) is first write, True(1) is second write.
        bool writeToggle{ false };
    };



    // All memory components accessible to the CPU through PPU memory mapped registers
    struct PPUMemoryComponents{
        PPURegisters memoryMappedRegisters{};

        // memory used to render sprites in a given frame/scanline
        SpriteMemory spriteMemory{};
        BackgroundTileMemory backgroundTileMemory{};

        // left and right pattern tables to be combined with attributes in the nametable table data to get color value
        PatternTable patternTables[2]{};
        NameTable nameTables[2]{};

        SystemColorPalette colorPalette{};
    };
}