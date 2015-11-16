#include "PPUComponents.h"

namespace NES {
    /////////////////////////////////////////////////////////////////
    // Control Register Accessors

    uint8_t PPURegisters::getNameTable() {
        return (uint8_t)(control & 0x03);
    }

    void PPURegisters::setNameTable(uint8_t table) {
        if (table <= 3) {
            control &= (0xfc + table);
        }
    }

    void PPURegisters::setIncrementMode(IncrementMode mode) {
        control &= (mode == IncrementMode::ADD_ONE ? 0xfb : 0xff);
    }

    IncrementMode PPURegisters::getIncrementMode() {
        return (control & 0x04) > 0 ? IncrementMode::ADD_32 : IncrementMode::ADD_ONE;
    }

    void PPURegisters::setSpritePatternTable(uint8_t table) {
        if (table <= 1) {
            control &= (table << 3);
        }
    }
    uint8_t PPURegisters::getSpritePatternTable() {
        return (uint8_t) ((control & 0x08) > 0);
    }

    void PPURegisters::setBackgroundPatternTable(uint8_t table) {
        if (table <= 1) {
            control &= (table << 4);
        }
    }

    uint8_t PPURegisters::getBackgroundPatternTable() {
        return  (uint8_t) ((control & 0x10) > 0);
    }

    void PPURegisters::setSpriteSize(SpriteSize spriteSize) {
        control &= (spriteSize == SpriteSize::SIZE_8_8 ? 0xef : 0xff);
    }

    SpriteSize PPURegisters::getSpriteSize() {
        return ((control & 0x20) > 0) ? SpriteSize::SIZE_8_16 : SpriteSize::SIZE_8_8;
    }

    void PPURegisters::setMasterSlaveSelect(MasterSlaveSelectMode mode) {
        control &= (mode == MasterSlaveSelectMode::USE_EXT ? 0xbf : 0xff);
    }

    MasterSlaveSelectMode PPURegisters::getMasterSlaveSelect() {
        return (control & 0x40) > 0 ? MasterSlaveSelectMode::USE_SLAVE : MasterSlaveSelectMode::USE_EXT;
    }

    void PPURegisters::setGenerateVBlankNmi(bool enabled) {
        control &= (enabled ? 0xff : 0x7f);
    }

    bool PPURegisters::getGenerateVBlankNmi() {
        return (control & 0x80) > 0;
    }

    /////////////////////////////////////////////////////////////////
    // Mask register accessor
    void PPURegisters::setGrayscale(bool enabled) {
        mask &= (enabled ? 0xff : 0xfe);
    }

    bool PPURegisters::getGrayscale() {
        return (mask & 0x01) > 0;
    }


    void PPURegisters::setShowBackgroundLeft(bool enabled) {
        mask &= (enabled ? 0xff : 0xfd);

    }

    bool PPURegisters::getShowBackgroundLeft() {
        return (mask & 0x02) > 0;
    }


    void PPURegisters::setShowSpritesLeft(bool enabled) {
        mask &= (enabled ? 0xff : 0xfb);
    }

    bool PPURegisters::getShowSpritesLeft() {
        return (mask & 0x04) > 0;
    }


    void PPURegisters::setShowBackground(bool enabled) {
        mask &= (enabled ? 0xff : 0xf7);
    }

    bool PPURegisters::getShowBackground() {
        return (mask & 0x08) > 0;
    }


    void PPURegisters::setShowSprites(bool enabled) {
        mask &= (enabled ? 0xff : 0xef);

    }

    bool PPURegisters::getShowSprites() {
        return (mask & 0x10) > 0;
    }


    void PPURegisters::setColorEmphasis(ColorEmphasis emphasis) {
        // emphasis already is set to the proper bits.  Adding in 0x0f to create the full bit mask
        mask &= (emphasis + 0x0f);
    }

    bool PPURegisters::getColorEmphasis(ColorEmphasis emphasis) {
        return (mask & emphasis) > 0;
    }

}