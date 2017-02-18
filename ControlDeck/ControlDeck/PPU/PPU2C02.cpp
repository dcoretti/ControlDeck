#include "ppu2c02.h"
#include "../common.h"

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
        if (curScanLine == 0 || curScanLine == 262) {
            // Pre-Render Cycle
        }
        
        else if (curScanLine < 241) {
            // Visible scan line rendering (240 scan lines)
            // visible scan line
            if (scanLineCycle == 0) {
                // Idle cycle
            } else if (scanLineCycle < 257) {
                // move shift registers here after first write cycle, meaning scanLineCycle >1
                if (scanLineCycle > 1) {
                    handleScrolling();
                }

                int state = (scanLineCycle - 1) % 8;
                // Fetch Name table byte
                if (state == 0) {
                    // see: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
                    // $2000 base addr + nameTable,x,yoffset
                    ppuAddr = nameTableBaseAddr | 0xfff;
                } else if (state == 1) {
                    currentNameTable = memoryMap->getByte(ppuAddr);

                }
                // Fetch Attribute table byte
                else if (state == 2) {
                    // Attribute table address in form 110 NN 1111 YYY XXX (attrTableBase + 1024 * nameTable + x-yRowOffsets)
                    ppuAddr = attributeTableBaseAddr |
                        (renderingRegisters.getNameTableSelect() << 10) |
                        ((renderingRegisters.getCoarseYScroll() >> 2) << 3) |
                        (renderingRegisters.getCoarseXScroll() >> 2);
                } else if (state == 3) {
                    attrTableEntry = memoryMap->getByte(ppuAddr);
                }
                // Fetch pattern table left half (high bit)
                else if (state == 4) {
                    ppuAddr = (uint16_t)ppuMemory.memoryMappedRegisters.getBackgroundPatternTable() * sizeof(PatternTable) + currentNameTable * sizeof(PatternTableEntry)
                        + renderingRegisters.getFineYScroll();
                } else if (state == 5) {
                    patternR = memoryMap->getByte(ppuAddr);

                }
                // Fetch pattern table right half (low bit)
                else if (state == 6) {
                    ppuAddr = (uint16_t)ppuMemory.memoryMappedRegisters.getBackgroundPatternTable() * sizeof(PatternTable) + currentNameTable * sizeof(PatternTableEntry)
                        + renderingRegisters.getFineYScroll() + 8;
                } else {
                    patternL = memoryMap->getByte(ppuAddr);
                }
            } else if (scanLineCycle < 321) {
                // sprite data for next scan line fetched here

            } else if (scanLineCycle < 337) {
                // Fetch background data for the next scan line
                handleScrolling();
            }
            // 337-340
            else {
                if (scanLineCycle == 337) {
                    handleScrolling();
                }

                // fetch nametableByte twice
            }
        } else if (curScanLine == 241) {
            // Post-render scan line
        } else {
            // Vblank period (20 scan lines) # 242-261
            if (scanLineCycle == 1 && ppuMemory.memoryMappedRegisters.getVBlank()) {
                // Second cycle enables vblank NMI!
            }
        }






    


        cycle++;
        if (cycle % cyclesPerScanLine == 0) {
            curScanLine = (curScanLine + 1) % scanLines;
        }
        scanLineCycle = cycle % cyclesPerScanLine;
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
                    atSubTile >> 4;
                }
                if (renderingRegisters.getCoarseXScroll & 2) {
                    atSubTile >> 2;
                }
                bkrndTileMemory.attrTile = atSubTile;

                // TODO sprite portion
            }

        }
    }
}