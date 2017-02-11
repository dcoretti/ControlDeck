#include "gtest/gtest.h"

#include "PPU/PPUComponents.h"
#include "PPU/PPUMemoryMap.h"

using NES::PPUMemoryMap;
using NES::PPUMemoryComponents;

class PPUMemoryMapperTest : public testing::Test {
protected:
    virtual void SetUp() {
        ppuMemoryComponents = PPUMemoryComponents();
        ppuMemoryMap.ppuComponents = &ppuMemoryComponents;
    }
    PPUMemoryMap ppuMemoryMap;
    PPUMemoryComponents ppuMemoryComponents;
};



TEST_F(PPUMemoryMapperTest, testColorPalette) {
    ppuMemoryComponents.colorPalette.universalBackgroundColor = 1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            ppuMemoryComponents.colorPalette.backgroundPalettes[i].colorIndex[j] = 200 + (i * 4 + j);
            ppuMemoryComponents.colorPalette.spritePalette[i].colorIndex[j] = 100 + (i * 4 + j);
        }
    }

    for (int mirror = 0; mirror < 8; mirror++) {
        uint16_t backgroundPaletteBase = (uint16_t)0x3f01 + mirror * 0x20;  // 32bytes per mirror
        uint16_t spritePaletteBase = (uint16_t)0x3f11 + mirror * 0x20;
        for (int i = 0; i < 3; i++) {
            ppuMemoryComponents.colorPalette.unusedPaletteData[i] = 10 + i;
        }

        EXPECT_EQ(1, ppuMemoryMap.getByte(backgroundPaletteBase - 1));    // universal bkrnd
        // Background palette
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                uint16_t res = ppuMemoryMap.getByte(backgroundPaletteBase + i * 4 + j);
                if (j == 3) {
                    if (i == 3) {
                        EXPECT_EQ(1, res);    // universal bkrnd mirror

                    } else {
                        EXPECT_EQ(10 + i, res);    // unused components
                    }
                } else {
                    EXPECT_EQ(200 + (i * 4 + j), res);    // main palette
                }
            }
        }
        // Sprite palette
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i == 3 && j == 3) {
                    break;
                }
                uint16_t res1 = ppuMemoryMap.getByte(spritePaletteBase + i * 4 + j);
                if (j == 3) {
                    EXPECT_EQ(10 + i, res1);    // unused components
                } else {
                    EXPECT_EQ(100 + (i * 4 + j), res1);    // main palette
                }
            }
        }
    }
}