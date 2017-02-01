#include "PPUMemoryMap.h"
namespace NES {
    uint8_t PPUMemoryMap::getByte(uint16_t address) {
        // Cartridge-backed CHR-ROM is mapped here and bank-switched(if needed) via CPU memory
        if (address < 0x2000) {
            return cartridge->mmc->doPPUReadOperation(address, *cartridge);
        }
        // 0x2000 -> 0x2fff internal VRAM w/ name tables and mirroring
        // 0x300-0x3eff mirror of 0x2000-0x2eff
        // 0x3f00-3fff internal palette control



        // Addresses are mirrored from 0x4000 to 0x10000
        //address = address % 0x4000; 
        //if (address < patternTableBoundary) {
        //    if (address < 0x1000) {
        //    } else {

        //    }
        //} else if (address < nameTableBoundary) {

        //} else {    // pallet table

        //}
        // todo finish this.
        return 0;
    }

}