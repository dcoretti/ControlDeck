#include "PPU2C02.h"


namespace NES {
    uint8_t PPU2C02::getStatus() {
        // Reading $2002 is latched to firstwrite reset http://wiki.nesdev.com/w/index.php/PPU_scrolling#.242002_read
        renderingRegisters.writeToggle = false;
        return memoryMappedRegisters.status;
    }


    void PPU2C02::setControl(uint8_t val) {
        memoryMappedRegisters.control = val;
        renderingRegisters.onControlWrite(memoryMappedRegisters);
    }

    void PPU2C02::setMask(uint8_t val) {

    }

    void PPU2C02::setOamAddr(uint8_t val) {

    }

    void PPU2C02::setOamData(uint8_t val) {
        if (!memoryMappedRegisters.isRenderingEnabled()) {
            // writes increment oamaddr
            memoryMappedRegisters.oamData = val;
            memoryMappedRegisters.oamAddr++;
        }
        // oamaddr increments during rendering has some odd behavior.. Is this necessary to implement?
        // see: http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#OAM_data_.28.242004.29_.3C.3E_read.2Fwrite
    }

    
    uint8_t PPU2C02::getOamData() {
        // don't increment addr if in vblank or forced vblank (rendering disabled)
        if (!memoryMappedRegisters.getVBlank() && memoryMappedRegisters.isRenderingEnabled()) {
            memoryMappedRegisters.oamAddr++;
        }
        return memoryMappedRegisters.oamData;
    }
    

    void PPU2C02::setScroll(uint8_t val) {
        //http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
        renderingRegisters.writeToggle = !renderingRegisters.writeToggle;

        memoryMappedRegisters.scroll = val;
    }

    void PPU2C02::setAddress(uint8_t) {
        //http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
        renderingRegisters.writeToggle = !renderingRegisters.writeToggle;

    }

    // $2007  CPU Memory mapped registers to communicate with PPU
    void PPU2C02::setData(uint8_t val) {

        // If rendering disabled, r/w to data will add 1/32 to v depending on $2000 vram increment bit
        // if endabled coarse x increment/ yincrement done with normal wrapping.
        memoryMappedRegisters.data = val;
        renderingRegisters.onDataAccess(memoryMappedRegisters);
    }
    uint8_t PPU2C02::getData() {
        // If rendering disabled, r/w to data will add 1/32 to v depending on $2000 vram increment bit
        // if endabled coarse x increment/ yincrement done with normal wrapping.
        renderingRegisters.onDataAccess(memoryMappedRegisters);
        return memoryMappedRegisters.data;
    }

    void PPU2C02::setOamDma(uint8_t val) {

    }
}