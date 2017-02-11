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
        } else if (scanLineCycle <= 256) {
            // fetch tiles

            // get nametable byte
            ppuMemory.memoryMappedRegisters.getNameTableBaseAddr();// +increment for nametable ? ? ;
            // get attribut table byte
            //tile bitmap low
            // tile bitmap high (low+8bytes)

        }// else if (scanLineCycle)


    }
}