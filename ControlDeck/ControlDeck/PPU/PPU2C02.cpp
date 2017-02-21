#include "ppu2c02.h"
#include "../common.h"

namespace NES {
    void Ppu2C02::setPowerUpState() {
        ppuMemory = PPUMemoryComponents();
        renderingRegisters = PPURenderingRegisters();
    }

    const uint32_t cyclesPerScanLine = 341;
    const uint32_t scanLines = 262; // some documentation starts at -1 but this starts at 0

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

    void Ppu2C02::fetchNameTableByte() {
        int state = scanLineCycle % 8;
        if (state == 1) {

        } else if (state == 2) {

        } else {
            DBG_CRASH("Somehow got called to fetchNameTableByte on cycle other than 1,2 scanLineCycle: %d", scanLineCycle);
        }
    } 

    RenderState Ppu2C02::getRenderState() {
        if (curScanLine == 0 || curScanLine == 262) {
            return RenderState::PreRenderScanLine;
        } else if (curScanLine < 241) {
            return RenderState::VisibleScanLines;
        } else if (curScanLine == 241) {
            return RenderState::PostRenderScanLine;
        } else { // curScanLine 242-261
            return RenderState::VerticalBlank;
        }
    }

    ////////////////////////////////////////////////////////////
    // TODO put ppuData value in the correct shift register  for bkrndTileMemory/spriteMemory
    // I think fetches will always happen for the NEXT draw so pixels are shifted off existing data while the next register is filled in??
    // from wiki: The data fetched from these accesses is placed into internal latches, and then fed to the appropriate shift registers when it's time to do so (every 8 cycles).
    // The shifters are reloaded during ticks 9, 17, 25, ..., 257. 

    static const uint16_t nameTableBaseAddr = 0x2000;
    static const uint16_t attributeTableBaseAddr = 0x23c0;
    void Ppu2C02::doPpuCycle() {
        // scan line is 341 ppu clock cycles (113.667 cpu cycles with a 3x multiplier of clock from cpu to ppu)
        // 260 scan lines visible, +2  (-1, 261) which are pre-render scanlines.
        RenderState renderState = getRenderState();
        if (renderState == RenderState::PreRenderScanLine) {

            if (scanLineCycle >= 257 && scanLineCycle <= 320) {
                ppuMemory.memoryMappedRegisters.oamAddr = 0;
            }
            // Pre-Render Cycle
            // Do scan line reads based on even / odd frames
            // Read operations still occur (can be read externally?)
        } else if (renderState == RenderState::VisibleScanLines) {
            // Visible scan line rendering (240 scan lines)
            // visible scan line
            if (scanLineCycle == 0) {
                // Idle cycle
            } 
            // Load a tile
            else if (scanLineCycle < 257 || scanLineCycle >= 321 && scanLineCycle < 337) {
                int state = (scanLineCycle - 1) % 8;
                // move shift registers here after first write cycle, meaning scanLineCycle >1
                if (scanLineCycle > 1) {
                    handleScrolling();
                }

                // Fetch Name table byte
                if (state == 0) {
                    // see: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
                    // $2000 base addr + nameTable,x,yoffset
                    ppuAddr = nameTableBaseAddr | 0xfff;
                } else if (state == 1) {
                    currentNameTable = getByte(ppuAddr);

                }
                // Fetch Attribute table byte
                else if (state == 2) {
                    // Attribute table address in form 110 NN 1111 YYY XXX (attrTableBase + 1024 * nameTable + x-yRowOffsets)
                    ppuAddr = attributeTableBaseAddr |
                        (renderingRegisters.getNameTableSelect() << 10) |
                        ((renderingRegisters.getCoarseYScroll() >> 2) << 3) |
                        (renderingRegisters.getCoarseXScroll() >> 2);
                } else if (state == 3) {
                    attrTableEntry = getByte(ppuAddr);
                }
                // Fetch pattern table left half (low bit)
                else if (state == 4) {
                    ppuAddr = (uint16_t)ppuMemory.memoryMappedRegisters.getBackgroundPatternTable() * sizeof(PatternTable) + currentNameTable * sizeof(PatternTableEntry)
                        + renderingRegisters.getFineYScroll();
                } else if (state == 5) {
                    patternL |= (uint16_t)getByte(ppuAddr) << 8;

                }
                // Fetch pattern table right half (high bit)
                else if (state == 6) {
                    ppuAddr = (uint16_t)ppuMemory.memoryMappedRegisters.getBackgroundPatternTable() * sizeof(PatternTable) + currentNameTable * sizeof(PatternTableEntry)
                        + renderingRegisters.getFineYScroll() + 8;
                } else {
                    patternR |= (uint16_t)getByte(ppuAddr) << 8;
                }

                Pixel pixel = getScreenPixel();
                renderBuffer.putPixel(scanLineCycle, curScanLine, pixel);
            } else if (scanLineCycle < 321) {
                ppuMemory.memoryMappedRegisters.oamAddr = 0;
                // sprite data for next scan line fetched here

            }
            // 337-340
            else {
                // fetch nametableByte twice
                if (scanLineCycle == 337) {
                    handleScrolling();
                }
                int state = 337 - scanLineCycle;
                // Fetch Name table byte
                if (state % 2 == 0) {
                    // see: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
                    // $2000 base addr + nameTable,x,yoffset
                    ppuAddr = nameTableBaseAddr | 0xfff;
                } else {
                    currentNameTable = getByte(ppuAddr);
                }
            }
        } else if (renderState == RenderState::PostRenderScanLine) {
            // Post-render scan line
        } else {    // RenderState::VerticalBlank
            // Vblank period (20 scan lines) # 242-261
            if (scanLineCycle == 1 && ppuMemory.memoryMappedRegisters.getGenerateVBlankNmi()) {
                // Second cycle enables vblank NMI!
                ppuMemory.memoryMappedRegisters.setVBlank(true);
            }
        }

        cycle++;
        if (cycle % cyclesPerScanLine == 0) {
            curScanLine = (curScanLine + 1) % scanLines;
        }
        scanLineCycle = cycle % cyclesPerScanLine;
    }


    void loadTile() {

    }

    void Ppu2C02::handleScrolling() {
        if (scanLineCycle > 1 && scanLineCycle < 258 || scanLineCycle > 321 && scanLineCycle < 338) {
            bkrndTileMemory.patternTableL = bkrndTileMemory.patternTableL << 1;
            bkrndTileMemory.patternTableR = bkrndTileMemory.patternTableR << 1;

            // Reload registers with data loaded during the last 8 cycles
            if (scanLineCycle % 8 == 1) {
                DBG_ASSERT((bkrndTileMemory.patternTableL & 0x00ff) == 0, "expected background shift register L to be 0 on lsb at this point. Instead was $%04x", bkrndTileMemory.patternTableL);
                DBG_ASSERT((bkrndTileMemory.patternTableR & 0x00ff) == 0, "expected background shift register R to be 0 on lsb at this point. Instead was $%04x", bkrndTileMemory.patternTableR);
                bkrndTileMemory.patternTableL |= patternL;
                bkrndTileMemory.patternTableR |= patternR;

                // assemble attribute register from coarse x, y
                // Gets the 2 bits associated with a 2x2 region of an at block representing 4x4 tiles of 8x8pixels each. This means the 2x2 tile is a 16x16px
                // region of the screen which has to have the same most significant bits of the palette color.  The lower 2 bits of the coarseX/Y represent a 2x2 region.
                // I don't really understand why some sources show a -1 on the coarseX scroll here... Others do something closer to below
                //uint8_t atSubTile = attrTableEntry >> ((renderingRegisters.getCoarseYScroll() & 2) << 1) | ((renderingRegisters.getCoarseXScroll()) & 2);
                uint8_t atSubTile = attrTableEntry;
                if (renderingRegisters.getCoarseYScroll() & 2) {
                    atSubTile >>= 4;
                }
                if (renderingRegisters.getCoarseXScroll() & 2) {
                    atSubTile >>= 2;
                }
                bkrndTileMemory.attrTile = atSubTile;

                // TODO sprite portion
            }

        }
    }

    Pixel Ppu2C02::getScreenPixel() {
        uint8_t bkrndIndex = (bkrndTileMemory.attrTile << 2) | ((bkrndTileMemory.patternTableR & 1) << 1) | (bkrndTileMemory.patternTableL & 1);
        uint8_t spriteIndex = 0;

        uint8_t bkrndColor = ppuMemory.colorPalette.getBkrndColorIndex(bkrndIndex);
        uint8_t spriteColor = 0;    // ppuMemory.colorPalette.getSpriteColorIndex();
        SpritePriority spritePriority = SpritePriority::BEHIND_BACKGROUND; // Sprites not implemented

        // Determine pixel priority
        uint8_t outColor = bkrndColor;
        if (bkrndIndex == 0 && spriteIndex > 0) {
            outColor = spriteColor;
        } else if (spritePriority == SpritePriority::IN_FRONT_OF_BACKGROUND) {
            outColor = spriteColor;
        }

        // Map color palette index to RGB pixel
        return colorPaletteNtsc[outColor];
    }


    uint8_t Ppu2C02::readRegister(PPURegister ppuRegister) {
        uint8_t val = 0;
        // TODO handle the fact that the lead capacitance means that reading CTRL, MASK, OAMADDR, SCROLL, ADDR
        //  for normally write-only registers will return that latched value which decays at some rate.
        switch (ppuRegister) {
        case PPURegister::PPUCTRL:
            break;
        case PPURegister::PPUMASK:
            break;
        case PPURegister::STATUS:
            val = ppuMemory.memoryMappedRegisters.status;
            renderingRegisters.onStatusRead(ppuMemory.memoryMappedRegisters);           
            ppuMemory.memoryMappedRegisters.setVBlank(false);
            break;
        case PPURegister::OAM_ADDRESS:
            break;
        case PPURegister::OAM_DATA:
            // this should actually be set to the open bus bits latched due to the trace capacitance 
            val = ppuMemory.memoryMappedRegisters.oamData;
            // TODO during rendering this will contain the current oam data being used

            break;
        case PPURegister::SCROLL:
            break;
        case PPURegister::ADDRESS:
            break;
        case PPURegister::DATA:
            // Data is buffered on read when not reading from palette range $3f00-3fff
            val = ppuMemory.memoryMappedRegisters.data;
            ppuMemory.memoryMappedRegisters.data = getByte(renderingRegisters.vramAddress);
            if (isAddressInPaletteRange(renderingRegisters.vramAddress)) {
                // In the palette range, return the vram address immediately - don't require a dummy read from DATA
                val = ppuMemory.memoryMappedRegisters.data;
            }
            renderingRegisters.onDataAccess(ppuMemory.memoryMappedRegisters);
            break;
        }
        return val;
    }

    // Source: http://wiki.nesdev.com/w/index.php/PPU_registers
    // TODO maybe move the ppu interaction code out here since the registers don't really own any of that
    void Ppu2C02::writeRegister(PPURegister ppuRegister, uint8_t val) {
        RenderState renderState = getRenderState();

        switch (ppuRegister) {
        case PPURegister::PPUCTRL:
            //if (cycle < 30000) {
            //    // PPU warmup
            //    break;
            //}
            ppuMemory.memoryMappedRegisters.control = val;
            renderingRegisters.onControlWrite(ppuMemory.memoryMappedRegisters);
            break;
        case PPURegister::PPUMASK:
            ppuMemory.memoryMappedRegisters.mask = val;
            break;
        case PPURegister::STATUS:
            break;
        case PPURegister::OAM_ADDRESS:

            ppuMemory.memoryMappedRegisters.oamAddr = val;
            break;
        case PPURegister::OAM_DATA:
            // Handle edge cases of OAM_ADDRESS increment for writes during rendering/pre-rendering scan lines
            // this should only be allowed when rendering enabled and scan
            if (ppuMemory.memoryMappedRegisters.isRenderingEnabled() &&
                (renderState == RenderState::VisibleScanLines || renderState == RenderState::PreRenderScanLine)) {
                break;
            } else {
                ppuMemory.memoryMappedRegisters.oamData = val;
                // oamaddr increments during rendering has some odd behavior.. Is this necessary to implement?
                // see: http://wiki.nesdev.com/w/index.php/PPU_programmer_reference#OAM_data_.28.242004.29_.3C.3E_read.2Fwrite
                ppuMemory.memoryMappedRegisters.oamAddr++;
                ppuMemory.memoryMappedRegisters.onOamDataWrite();
            }

            break;
        case PPURegister::SCROLL:
            ppuMemory.memoryMappedRegisters.scroll = val;
            renderingRegisters.onScrollWrite(ppuMemory.memoryMappedRegisters);
            break;
        case PPURegister::ADDRESS:
            ppuMemory.memoryMappedRegisters.address = val;
            renderingRegisters.onAddressWrite(ppuMemory.memoryMappedRegisters);
            break;
        case PPURegister::DATA:
            ppuMemory.memoryMappedRegisters.data = val;
            renderingRegisters.onDataAccess(ppuMemory.memoryMappedRegisters);
            break;
        };
    }

    void Ppu2C02::doRegisterUpdates() {
        //if (ppuMemory.memoryMappedRegisters.isRenderingEnabled()) {

        //}
        RenderState renderState = getRenderState();
        if (renderState == RenderState::PreRenderScanLine) {
            if (scanLineCycle == 1) {
                // PPUSTATUS resets
                ppuMemory.memoryMappedRegisters.setSpriteOverflow(false);
                ppuMemory.memoryMappedRegisters.setVBlank(false);
                ppuMemory.memoryMappedRegisters.setSpriteZeroHit(false);

            }
            if (scanLineCycle >= 257 && scanLineCycle <= 320) {
                ppuMemory.memoryMappedRegisters.oamAddr = 0;
            }

        } else if (renderState == RenderState::VisibleScanLines) {
            if (scanLineCycle >= 257 && scanLineCycle <= 320) {
                ppuMemory.memoryMappedRegisters.oamAddr = 0;
            }
        }
    }

    /**
    *   General map of address space on PPU including cartridge-supplied memory
    *   ref: http://wiki.nesdev.com/w/index.php/PPU_memory_map
    **/
    uint8_t Ppu2C02::doMemoryOperation(uint16_t address, uint8_t write, bool read) {
        uint8_t *opAddr;
        address = address % 0x4000;

        // Cartridge-backed CHR-ROM is mapped here and bank-switched(if needed) via CPU memory
        if (address < 0x2000) {
            return cartridge->mmc->doCHRMemoryOperationOperation(*cartridge, address, write, read);
        }
        // either internal vram or cart ram to enable 4 nametables
        else if (address < 0x3f00) {
            // 0x3000-0x3eff is a mirror of 0x2000-0x2fff
            uint16_t base = (address - 0x2000) % 0x1000;   // 4 1k nametables mirrored up to 2eff
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
                opAddr = &ppuMemory.nameTables[table].nameTable[offset];
            } else {
                opAddr = &ppuMemory.nameTables[table].attributeTable.tileGroup[offset - 0x3c0];
            }
        }
        // Palette memory ($3f00-$3f20 mirrored up to $4000)
        else if (address < 0x4000) {
            uint8_t base = (address - 0x3f00) % 0x20;   // 32 bits mirrored
            if (base == 0 || base == 0x10) {
                // universal background'
                opAddr = &ppuMemory.colorPalette.universalBackgroundColor;
            }
            // unused in rendering normally and mirrored
            else if (base == 0x04 || base == 0x14) {
                opAddr = &ppuMemory.colorPalette.unusedPaletteData[0];
            } else if (base == 0x08 || base == 0x18) {
                opAddr = &ppuMemory.colorPalette.unusedPaletteData[1];
            } else if (base == 0x0c || base == 0x1c) {
                opAddr = &ppuMemory.colorPalette.unusedPaletteData[2];
            } else {
                int paletteNum = (base - 1) / 4;
                int colorIndexNum = (base - 1) % 4;
                if (base < 0x10) {
                    // Background palettes
                    opAddr = &ppuMemory.colorPalette.backgroundPalettes[paletteNum].colorIndex[colorIndexNum];
                } else {
                    // Sprite palettes
                    opAddr = &ppuMemory.colorPalette.spritePalette[paletteNum - 4].colorIndex[colorIndexNum];
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

    bool Ppu2C02::isAddressInPaletteRange(uint16_t address) {
        address = address % 0x4000;
        return address >= 0x3f00 && address < 0x4000;
    }
}