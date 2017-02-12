#include "PPUMemoryMap.h"
namespace NES {
    /**
    *   General map of address space on PPU including cartridge-supplied memory
    *   ref: http://wiki.nesdev.com/w/index.php/PPU_memory_map
    **/
    uint8_t PPUMemoryMap::doMemoryOperation(uint16_t address, uint8_t write, bool read) {
        uint8_t *opAddr;
        address = address % 0x4000;

        // Cartridge-backed CHR-ROM is mapped here and bank-switched(if needed) via CPU memory
        if (address < 0x2000) {
            return cartridge->mmc->doCHRMemoryOperationOperation(*cartridge, address, write, read);
        } 
        // either internal vram or cart ram to enable 4 nametables
        else if (address < 0x3f00) {
            // 0x3000-0x3eff is a mirror of 0x2000-0x2fff
            uint16_t base = (address - 0x2000)  % 0x1000;   // 4 1k nametables mirrored up to 2eff
            size_t table = base / 0x400;
            size_t offset = base % 0x400;

            // 4 tables addressable, only 2 in ram so mirror them based on cartridge settings.
            // See http://wiki.nesdev.com/w/index.php/Mirroring
            switch (cartridge->mirroring) {
            case PPUMirroring::PPU_VERTICAL:
                table = table % 2;  // Configuration: 0, 1, 0, 1 
                break;
            case PPUMirroring::PPU_HORIZONTAL:
                table = table / 2;  // Configuration: 0, 0, 1, 1
                break;
            default:
                DBG_CRASH("Unsupported mirroring mode found %d", cartridge->mirroring);
            };
            if (base % 0x400 < 0x3c0) {
                opAddr = &ppuComponents->nameTables[table].nameTable[offset];
            } else {
                opAddr = &ppuComponents->nameTables[table].attributeTable.tileGroup[offset - 0x3c0];
            }
        }
        // Palette memory ($3f00-$3f20 mirrored up to $4000)
        else if (address < 0x4000) {
            uint8_t base = (address - 0x3f00) % 0x20;   // 32 bits mirrored
            if (base == 0 || base == 0x10) {
                // universal background'
                opAddr = &ppuComponents->colorPalette.universalBackgroundColor;
            } 
            // unused in rendering normally and mirrored
            else if (base == 0x04 || base == 0x14) {
                opAddr = &ppuComponents->colorPalette.unusedPaletteData[0];
            } else if (base == 0x08 || base == 0x18) {
                opAddr = &ppuComponents->colorPalette.unusedPaletteData[1];
            } else if (base == 0x0c || base == 0x1c) {
                opAddr = &ppuComponents->colorPalette.unusedPaletteData[2];
            } else {
                int paletteNum = (base - 1) / 4;
                int colorIndexNum = (base - 1) % 4;
                if (base < 0x10) {
                    // Background palettes
                    opAddr = &ppuComponents->colorPalette.backgroundPalettes[paletteNum].colorIndex[colorIndexNum];
                } else {
                    // Sprite palettes
                    opAddr = &ppuComponents->colorPalette.spritePalette[paletteNum - 4].colorIndex[colorIndexNum];
                }
            }
        } else {
            DBG_CRASH("Unsupported memory operation on address %04x isRead %d write value %02x\n", address, read, write);
            return 0;   // for compiler warnings
        }

        uint8_t readResult = *opAddr;
        if (!read) {
            *opAddr = write;
        }
        return readResult;
    }

}