#include "PPUComponents.h"
#include "../common.h"

namespace NES {
    /////////////////////////////////////////////////////////////////
    // Control Register Accessors

    uint16_t PPURegisters::getNameTableBaseAddr() {
        // http://wiki.nesdev.com/w/index.php/PPU_registers#PPUCTRL
        return 0x2000 + (uint16_t)(getNameTable() * 0x400);
    }


    uint8_t PPURegisters::getNameTable() {
        return (uint8_t)(control & 0x03);
    }

    void PPURegisters::setNameTable(uint8_t table) {
        if (table <= 3) {
            control = (control & 0xfc) | table;
        }
    }

    uint8_t PPURegisters::getDataAccessIncrement() {
        return (control & 0x04) > 0 ? 32 : 1;
    }

    void PPURegisters::setSpritePatternTable(uint8_t table) {
        if (table <= 1) {
            control = (control & 0xf7) | (table <<3);
        }
    }
    uint8_t PPURegisters::getSpritePatternTable() {
        return (uint8_t) ((control & 0x08) > 0);
    }

    void PPURegisters::setBackgroundPatternTable(uint8_t table) {
        if (table <= 1) {
            control = (control & 0xef) | (table << 4);
        }
    }

    uint8_t PPURegisters::getBackgroundPatternTable() {
        return  (uint8_t) ((control & 0x10) > 0);
    }

    void PPURegisters::setSpriteSize(SpriteSize spriteSize) {
        control = (control & 0xef) | ((uint8_t)spriteSize << 6);
    }

    SpriteSize PPURegisters::getSpriteSize() {
        return ((control & 0x20) > 0) ? SpriteSize::SIZE_8_16 : SpriteSize::SIZE_8_8;
    }

    void PPURegisters::setMasterSlaveSelect(MasterSlaveSelectMode mode) {
        control = (control & 0xbf) | ((uint8_t)mode << 7);
    }

    MasterSlaveSelectMode PPURegisters::getMasterSlaveSelect() {
        return (control & 0x40) > 0 ? MasterSlaveSelectMode::USE_SLAVE : MasterSlaveSelectMode::USE_EXT;
    }

    void PPURegisters::setGenerateVBlankNmi(bool enabled) {
        control = (control & 0xef) | ((uint8_t)enabled << 7);
    }

    bool PPURegisters::getGenerateVBlankNmi() {
        return (control & 0x80) > 0;
    }

    /////////////////////////////////////////////////////////////////
    // Mask register accessor
    void PPURegisters::setGrayscale(bool enabled) {
        mask = (mask & 0xfe) | (uint8_t)enabled;
    }

    bool PPURegisters::getGrayscale() {
        return (mask & 0x01) > 0;
    }

    void PPURegisters::setShowBackgroundLeft(bool enabled) {
        mask = (mask & 0xfd) | ((uint8_t)enabled << 2);
    }

    bool PPURegisters::getShowBackgroundLeft() {
        return (mask & 0x02) > 0;
    }


    void PPURegisters::setShowSpritesLeft(bool enabled) {
        mask = (mask & 0xfb) | ((uint8_t)enabled << 3);
    }

    bool PPURegisters::getShowSpritesLeft() {
        return (mask & 0x04) > 0;
    }


    void PPURegisters::setShowBackground(bool enabled) {
        mask = (mask & 0xf7) | ((uint8_t)enabled << 3);
    }

    bool PPURegisters::getShowBackground() {
        return (mask & 0x08) > 0;
    }


    void PPURegisters::setShowSprites(bool enabled) {
        mask = (mask & 0xef) | ((uint8_t)enabled << 4);
    }

    bool PPURegisters::getShowSprites() {
        return (mask & 0x10) > 0;
    }


    void PPURegisters::setColorEmphasis(ColorEmphasis emphasis) {
        // emphasis already is set to the proper bits.  Adding in 0x0f to create the full bit mask
        mask = (mask & 0x1f) | ((uint8_t)emphasis << 5);
    }

    bool PPURegisters::getColorEmphasis(ColorEmphasis emphasis) {
        return (mask & emphasis) > 0;
    }


    // TODO take a look at this again... 1E means ALL rendering enabled, only bits 3/4 enable rendering
    // $00 means ALL disabled. I'm not sure this accessor is useful overall since only 0 means disabled.
    //// True if bits 1-4 are true in the mask register (show background or sprites)
    bool PPURegisters::isRenderingEnabled() {
        return (mask & 0x18) > 0;
    }


    /////////////////////////////////////////////////////////////////
    // Status register accessors

    void PPURegisters::setSpriteOverflow(bool overflow) {
        status = (status & 0xdf) | ((uint8_t)overflow << 6);
    }

    bool PPURegisters::getSpriteOverflow() {
        return (status & 0x20) > 0;
    }

    void PPURegisters::setSpriteZeroHit(bool hit) {
        status = (status & 0xbf) | ((uint8_t)hit << 6);
    }

    bool PPURegisters::getSpriteZeroHit() {
        return (status & 0x40) > 0;
    }

    void PPURegisters::setVBlank(bool vblank) {
        status = (status & 0x7f) | ((uint8_t)vblank << 7);
    }

    bool PPURegisters::getVBlank() {
        return (status & 0x80) > 0;
    }

    void PPURegisters::onStatusRead() {
        setVBlank(0);
    }

    void PPURegisters::onOamDataWrite() {
        // TODO write to OAM using newly set value on OAMData register
    }



    /////////////////////////////////////////////////////////////////
    // OAM attributes

    void ObjectAttributeMemory::setPalette(uint8_t palette) {
        if (palette < 4) {
            attributes = (attributes & 0xfc) | palette;
        }
    }

    uint8_t ObjectAttributeMemory::getPalette() {
        return (uint8_t)(attributes & 0x03);
    }

    void ObjectAttributeMemory::setPriority(SpritePriority priority) {
        attributes = (attributes & 0xdf) | ((uint8_t)priority << 5);
    }

    SpritePriority ObjectAttributeMemory::getPriority() {
        return (attributes & 0x20) > 0 ? SpritePriority::BEHIND_BACKGROUND : SpritePriority::IN_FRONT_OF_BACKGROUND;
    }

    void ObjectAttributeMemory::setHorizontalFlip(bool enabled) {
        attributes = (attributes & 0xbf) | ((uint8_t)enabled << 6);
    }

    bool ObjectAttributeMemory::getHorizontalFlip() {
        return (attributes & 0x40) > 0;

    }

    void ObjectAttributeMemory::setVerticalFlip(bool enabled) {
        attributes = (attributes & 0x7f) | ((uint8_t)enabled << 7);
    }

    bool ObjectAttributeMemory::getVerticalFlip() {
        return (attributes & 0x80) > 0;
    }

    uint8_t ObjectAttributeMemory::getTileIndex(bool is8x16) {
        return is8x16 ? tileIndex >> 1 : tileIndex;
    }

    uint8_t ObjectAttributeMemory::getPatternTableFor8x16() {
        return tileIndex & 0x01;
    }


    /////////////////////////////////////////////////////////////////
    // Name/Pattern table
    /**
    *   Tile bit layout: 33221100 within each 4x4 tile sub-group
    *   Returns the 2bit number representing those bits.
    */
    uint8_t AttributeTable::getTileBitsFromGroup(uint8_t group, uint8_t tile) {
        if (tile < 4 && group < 64) {
            return (tileGroup[group] & (0x03 << tile)) >> tile;
        }
        return 0;
    }

    /////////////////////////////////////////////////////////////////
    // CPU main register <-> Rendering register callbacks
    //
    // details: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls
    void PPURenderingRegisters::onStatusRead(PPURegisters &registers) {
        writeToggle = 0;
    }


    void PPURenderingRegisters::onControlWrite(PPURegisters &registers) {
        tempVramAddress &= 0xf3ff;  // clear nametable bits (10-11)
        uint8_t nameTable = registers.getNameTable();
        tempVramAddress |= (uint16_t) nameTable << 10;
    }

    void PPURenderingRegisters::onScrollWrite(PPURegisters &registers) {
        if (writeToggle) {
            tempVramAddress &= 0xffe0;
            tempVramAddress |= (registers.scroll >> 3);
            fineXScroll = registers.scroll & 0x07;
        } else {
            tempVramAddress &= 0x8c1f;
            tempVramAddress |= ((registers.scroll & 0x07) << 12);
            tempVramAddress |= ((registers.scroll & 0xf8) << 2);          
        }
        writeToggle = !writeToggle;
    }

    void PPURenderingRegisters::onAddressWrite(PPURegisters &registers) {
        if (writeToggle) {
            // T/V registers have 15 bits, not 16 and only 14 are set via the ADDRESS register
            tempVramAddress &= 0x00ff;  
            tempVramAddress |= ((registers.address & 0x3f) << 8);
        } else {
            tempVramAddress &= 0xff00;
            tempVramAddress |= registers.address;
            vramAddress = tempVramAddress;
        }
        writeToggle = !writeToggle;
    }

    static const uint16_t coarseYMask = 0x03e0;
    static const uint16_t fineYMask = 0x7000;
    static const uint16_t coarseXMask = 0x001f;
    static const uint16_t nameTableSelectMask = 0x0c00;

    void PPURenderingRegisters::onDataAccess(PPURegisters &registers) {
        if (registers.isRenderingEnabled()) {
            // Code edge cases on increment from http://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
            // Coarse X increment / wraparound
            uint16_t coarseX = getCoarseXScroll();

            if (coarseX == coarseXMask) {
                vramAddress &= ~0x001f;
                // switch horizontal nametable
                vramAddress ^= 0x0400;
            } else {
                vramAddress++;
            }

            // Fine and Coarse Y increment / wraparound
            // decompose V
            uint16_t coarseY = getCoarseYScroll();
            uint16_t fineY = getFineYScroll();
            if (fineY < 7) {
                fineY++;
            } else {
                fineY = 0;
                // wrap coarse Y since name tables only have 30 rows (0-29)
                if (coarseY == 29) {
                    coarseY = 0;
                    // switch vertical name table
                    vramAddress ^= 0x0800;
                } else if (coarseY == 31) {
                    // This can happen if coarse Y is set out of bounds through $2005 scroll register
                    // Don't switch name table.
                    coarseY = 0;
                } else {
                    coarseY++;
                }

                // put coarse / fine Y back.
                setCoarseY(coarseY);
                setFineY(fineY);
            }
        } else {
            vramAddress += registers.getDataAccessIncrement();
        }
    }

    void PPURenderingRegisters::setCoarseY(uint16_t coarseY) {
        DBG_ASSERT(coarseY < 32, "Invalid coarse Y of %d found.  Expected to fit within 5 bits", coarseY);
        vramAddress = (vramAddress & ~coarseYMask) | (coarseY << 5);
    }

    void PPURenderingRegisters::setFineY(uint16_t fineY) {
        DBG_ASSERT(fineY < 8, "Invalid coarse Y of %d found.  Expected to fit within 5 bits", fineY);
        vramAddress = (vramAddress & ~fineYMask) | (fineY << 12);
    }


    uint16_t PPURenderingRegisters::getCoarseXScroll() {
        return (vramAddress & coarseXMask);
    }

    uint16_t PPURenderingRegisters::getCoarseYScroll() {
        return (vramAddress & coarseYMask) >> 5;
    }

    uint16_t PPURenderingRegisters::getNameTableSelect() {
        return (vramAddress & nameTableSelectMask) >> 10;
    }

    uint16_t PPURenderingRegisters::getFineYScroll() {
        return (vramAddress & fineYMask) >> 12;
    }

    uint8_t SystemColorPalette::getBkrndColorIndex(uint8_t index) {
        uint8_t paletteNum = index & 0xc;
        uint8_t pixelVal = index & 3;

        if (index == 0) {
            return universalBackgroundColor;
        } else {
            return backgroundPalettes[paletteNum].colorIndex[pixelVal];
        }
    }

    uint8_t SystemColorPalette::getSpriteColorIndex(uint8_t index) {
        return 0;
    }
}