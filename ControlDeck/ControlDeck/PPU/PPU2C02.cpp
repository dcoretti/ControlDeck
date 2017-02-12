#include "ppu2c02.h"


namespace NES {
    void Ppu2C02::setPowerUpState() {
        ppuMemory = PPUMemoryComponents();
        renderingRegisters = PPURenderingRegisters();
    }


    uint8_t Ppu2C02::getStatus() {
        // Reading $2002 is latched to firstwrite reset http://wiki.nesdev.com/w/index.php/PPU_scrolling#.242002_read
        renderingRegisters.writeToggle = false;
        return ppuMemory.memoryMappedRegisters.status;
    }


    void Ppu2C02::setControl(uint8_t val) {
        ppuMemory.memoryMappedRegisters.control = val;
        renderingRegisters.onControlWrite(ppuMemory.memoryMappedRegisters);
    }

    void Ppu2C02::setMask(uint8_t val) {

    }

    void Ppu2C02::setOamAddr(uint8_t val) {

    }

    void Ppu2C02::setOamData(uint8_t val) {
        if (!ppuMemory.memoryMappedRegisters.isRenderingEnabled()) {
            // writes increment oamaddr
            ppuMemory.memoryMappedRegisters.oamData = val;
            ppuMemory.memoryMappedRegisters.oamAddr++;
        }
        // oamaddr increments during rendering has some odd behavior.. Is this necessary to implement?
        // see: http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#OAM_data_.28.242004.29_.3C.3E_read.2Fwrite
    }

    
    uint8_t Ppu2C02::getOamData() {
        // don't increment addr if in vblank or forced vblank (rendering disabled)
        if (!ppuMemory.memoryMappedRegisters.getVBlank() && ppuMemory.memoryMappedRegisters.isRenderingEnabled()) {
            ppuMemory.memoryMappedRegisters.oamAddr++;
        }
        return ppuMemory.memoryMappedRegisters.oamData;
    }
    

    void Ppu2C02::setScroll(uint8_t val) {
        //http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
        renderingRegisters.writeToggle = !renderingRegisters.writeToggle;

        ppuMemory.memoryMappedRegisters.scroll = val;
    }

    void Ppu2C02::setAddress(uint8_t) {
        //http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
        renderingRegisters.writeToggle = !renderingRegisters.writeToggle;

    }

    // $2007  CPU Memory mapped registers to communicate with PPU
    void Ppu2C02::setData(uint8_t val) {

        // If rendering disabled, r/w to data will add 1/32 to v depending on $2000 vram increment bit
        // if endabled coarse x increment/ yincrement done with normal wrapping.
        ppuMemory.memoryMappedRegisters.data = val;
        renderingRegisters.onDataAccess(ppuMemory.memoryMappedRegisters);
    }
    uint8_t Ppu2C02::getData() {
        // If rendering disabled, r/w to data will add 1/32 to v depending on $2000 vram increment bit (vertical or horizontal scrolling)
        // if endabled coarse x increment/ yincrement done with normal wrapping.
        renderingRegisters.onDataAccess(ppuMemory.memoryMappedRegisters);
        return ppuMemory.memoryMappedRegisters.data;
    }

    void Ppu2C02::setOamDma(uint8_t val) {

    }


    // http://nesdev.com/2C02%20technical%20reference.TXT
    // clock signal is main 6502 clock (21.48mhz / 4)'
    // 341 ppu clock cycles per scan line
    // 240 scan lines per frame + 20 pre-render + 1 dummy + 1 post-render = 262 total scan lines per frame
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
    const uint32_t cyclesPerScanLine = 341;
    const uint32_t scanLines = 262;
    void Ppu2C02::doPpuCycle() {

        // scan line is 341 ppu clock cycles (113.667 cpu cycles with a 3x multiplier of clock from cpu to ppu)
        // 260 scan lines visible, +2  (-1, 261) which are pre-render scanlines.

        // 0 - pre-render dummy scan line 
        // 1 first visible
        //240 first last visible
        // 241 post-render scan line
        // 242-261 vblank
        // 262 - pre-render dummy scan line


        // scan lines 0- 

        if (cycle> 0 &&cycle % cyclesPerScanLine == 0) {
            scanLine++;
            if (scanLine >= scanLines) {
                scanLine = 0;
            }
        }
        cycle++;

    }


    void Ppu2C02::handleVisibleScanLine() {
        if (scanLineCycle == 0) {
            // idle cycle
        } else if (scanLineCycle < 257) {
            // fetch tiles

            // get nametable byte
            ppuMemory.memoryMappedRegisters.getNameTableBaseAddr();// +increment for nametable ? ? ;
            // get attribut table byte
            //tile bitmap low
            // tile bitmap high (low+8bytes)

        } else if (scanLineCycle <= 320) {

        } else if (scanLineCycle < 337)


    }
}