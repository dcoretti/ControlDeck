#pragma once
#include "PPUComponents.h"
#include "PPUMemoryMap.h"

namespace NES {
    /**
    *   DMA state 
    */
    struct OamDmaState {
        void activateDma(uint8_t pageAddr, bool oddCycle);
        void doDmaAction();

        // Copy one page (256 bytes)
        static const int amountToCopy = 256;

        bool isDmaActive{ false };
        uint8_t curPageOffset{ 0 };
        uint8_t pageAddr{ 0 };
        // read/write toggle
        bool isRead{ true };
        // Add an extra cycle if DMA triggered on an odd cpu cycle
        bool addOddCycle{ false };


    };

    // Starting cycle in scan line state
    enum class ScanLineState {
        Idle = 0,                 //0
        TileFetch = 1,            //1-256
        SpriteFetch = 257,        //257-320
        FirstTwoTileFetch = 321,  //321-336
        NameTableByteFetch = 337, //337-340
    };

    // Frame rendering state transitions between scan lines for a total of 262 scan lines per frame, 240 of which
    // are visible.
    enum class RenderState {
        VisibleScanLines = 0,       //0-239
        PostRenderScanLine = 240,   //240
        VerticalBlank = 241,        //241-260
        PreRenderScanLine = 261,    //261
    };

    class Ppu2C02 {
    public:
        // Memory mapped registers connected to CPU
        uint8_t getStatus();
        void setControl(uint8_t val);

        void setMask(uint8_t val);

        void setOamAddr(uint8_t val);

        void setOamData(uint8_t val);
        uint8_t getOamData();

        void setScroll(uint8_t);

        void setAddress(uint8_t);

        void setData(uint8_t val);
        uint8_t getData();

        void setOamDma(uint8_t val);

        void setPowerUpState();

        void doPpuCycle();

        void handleScrolling();

        // Registers accessible to CPU through memory mapper
        PPUMemoryComponents ppuMemory{};

        // Internal rendering state registers/memory (not directly connected to memory mapper)
        // used to render sprites in a given frame/scanline
        PPURenderingRegisters renderingRegisters{};
        SpriteMemory spriteMemory{};
        BackgroundTileMemory bkrndTileMemory{};

        uint16_t ppuAddr;
        uint8_t ppuData;
        PPUMemoryMap *memoryMap;

    private:
        uint16_t curScanLine{ 0 };
        uint32_t cycle{ 0 };
        uint16_t scanLineCycle{ 0 };    // one cycle per pixel

        // Scan line produced data pending load into registers for rendering
        uint16_t currentNameTable{ 0 };
        uint8_t patternL{ 0 };  // current high bit pattern table entry
        uint8_t patternR{ 0 };  // current low bit pattern table entry
        uint8_t attrTableEntry{ 0 };


        /**
        *   Iterate primary OAM to determine which objects are in Y-range for the NEXT scan line from highest priority (0)
        *   to lowest.  Set sprite overflow flag in status register if more than 8 are found.
        *   See "In-range object evaluation" http://nesdev.com/2C02%20technical%20reference.TXT
        */
        void populateSecondaryOam();

        /**
        *   Set up pattern table data for sprites drawn on the next scan line.
        *   For each sprite (8 total)
        *   1-2.   ?? Reuse of hardware for playfield/background population causes wasted cycles
        *   3.  pattern table bitmap 0 fetch for sprite
        *   4.  pattern table bitmap 1 fetch for sprite
        *   
        *   cycles 129-160
        */
        void populateSpritePatterns();

        /**
        *   Fetch background data for first two tiles to be rendered on next scanline (BackgroundLineContext).
        *   For each tile (2 total)
        *   1. Get name table byte
        *   2. Get attribute table byte
        *   3. Get pattern table bitmap 0
        *   4. Get pattern table bitmap 1

        * cycle 161-168
        */
        void populateBackgroundPatterns();


        void fetchNameTableByte();

        bool isDmaActive;
    };
}