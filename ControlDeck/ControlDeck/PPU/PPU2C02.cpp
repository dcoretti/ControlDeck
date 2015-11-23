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

    }

    /*
    uint8_t PPU2C02::getOamData() {

    }
    */

    void PPU2C02::setScroll(uint8_t) {
        //http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
        renderingRegisters.writeToggle = !renderingRegisters.writeToggle;


    }

    void PPU2C02::setAddress(uint8_t) {
        //http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
        renderingRegisters.writeToggle = !renderingRegisters.writeToggle;

    }

    // $2007
    void PPU2C02::setData(uint8_t val) {

        // If rendering disabled, r/w to data will add 1/32 to v depending on $2000 vram increment bit
        // if endabled coarse x increment/ yincrement done with normal wrapping.
    }
    uint8_t PPU2C02::getData() {
        // If rendering disabled, r/w to data will add 1/32 to v depending on $2000 vram increment bit
        // if endabled coarse x increment/ yincrement done with normal wrapping.
        renderingRegisters.onDataAccess(memoryMappedRegisters);
        return memoryMappedRegisters.data;
    }

    void PPU2C02::setrOamDma(uint8_t val) {

    }
}