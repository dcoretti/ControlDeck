#include "PPUComponents.h"

namespace NES {
    /////////////////////////////////////////////////////////////////
    // Control Register Accessors

    uint8_t PPURegisters::getNameTable() {
        return (uint8_t)(control & 0x03);
    }

    void PPURegisters::setNameTable(uint8_t table) {
        if (table <= 3) {
            control = (control & 0xfc) | table;
        }
    }

    void PPURegisters::setIncrementMode(IncrementMode mode) {
        control = (control & 0xfb) | ((uint8_t)mode << 2);

        control &= (mode == IncrementMode::ADD_ONE ? 0xfb : 0xff);
    }

    IncrementMode PPURegisters::getIncrementMode() {
        return (control & 0x04) > 0 ? IncrementMode::ADD_32 : IncrementMode::ADD_ONE;
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
}