#pragma once
#include "PPUComponents.h"

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

		PPURegisters memoryMappedRegisters;
		PPURenderingRegisters renderingRegisters;

    private:

        // http://nesdev.com/2C02%20technical%20reference.TXT
        // clock signal is main 6502 clock (21.48mhz / 4)'
        // 341 ppu clock cycles per scan line
        // 240 scan lines per frame + 20 pre-render + 1 dummy + 1 post-render = 262
        // Memory access is 2 cycles long

        /*
        *   Per scan line process (341 cycles)
        *   Note:
        *   - memory access can only be done every 2 cycles (170 accesses per scan line
        *   - one pixel rendered every cycle, the last cycle is no op.
        *   - prior scan line fetches the first two tiles for the current 
        *     (the first tile fetch for current scan line is the third actually used)
        *   - First draw at 16 minus the fine horizontal scroll value (0-7).
        *   - Each set of 8 horizontal pixels must use the same 3-color palette 
        *   - TODO scanline 20 is 340 cycles on every odd frame due to NTSC weirdness.. see "Extra cycle frames" in above ref.
        *   - TODO during reset/startup writes to control register are ignored for ~30k cycles (see reset sequences)
        *
        *   For each 32 tiles in a scanline
        *   Fetch (16 cycles - 4 reads)
        *       1.  fetch name table byte
        *       2. fetch attribute table byte
        *       3. fetch pattern table bitmap 0
        *       4. fetch pattern table bitmap 1
        *   
        *   For each scan line 
        *   Determine if each object in OAM (64) is in Y range for NEXT scan line (256 cycles total)
        *       (values for y-coords need to have ycoord-1 to ensure this) - 4 cycles
        *   
        *   Memory fetch - PPU retrieves appropriate pattern table data for the objects to be drawn on the next scan line
        */
           

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


        bool isDmaActive;
    };
}