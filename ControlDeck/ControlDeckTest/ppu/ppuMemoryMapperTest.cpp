#include "gtest/gtest.h"

#include "PPU/PPUComponents.h"
#include "PPU/PPU2C02.h"
#include "cartridge.h"
#include "common.h"

using NES::PPUMemoryComponents;
using NES::Cartridge;
using NES::PPUMirroring;
using NES::Ppu2C02;

class PPUMemoryMapperTest : public testing::Test {
protected:
    virtual void SetUp() {
        cart = Cartridge();
        ppu.cartridge = &cart;
    }

    void setNameTableData() {
        memset(ppu.ppuMemory.nameTables[0].nameTable, 0xAB, arrSizeof(ppu.ppuMemory.nameTables[0].nameTable));
        memset(ppu.ppuMemory.nameTables[0].attributeTable.tileGroup, 0xCD, arrSizeof(ppu.ppuMemory.nameTables[0].attributeTable.tileGroup));

        memset(ppu.ppuMemory.nameTables[1].nameTable, 0x12, arrSizeof(ppu.ppuMemory.nameTables[1].nameTable));
        memset(ppu.ppuMemory.nameTables[1].attributeTable.tileGroup, 0x34, arrSizeof(ppu.ppuMemory.nameTables[1].attributeTable.tileGroup));
    }

    Ppu2C02 ppu;
    Cartridge cart;
};



TEST_F(PPUMemoryMapperTest, testColorPalette) {
    ppu.ppuMemory.colorPalette.universalBackgroundColor = 1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            ppu.ppuMemory.colorPalette.backgroundPalettes[i].colorIndex[j] = 200 + (i * 4 + j);
            ppu.ppuMemory.colorPalette.spritePalette[i].colorIndex[j] = 100 + (i * 4 + j);
        }
    }

    for (int mirror = 0; mirror < 8; mirror++) {
        uint16_t backgroundPaletteBase = (uint16_t)0x3f01 + mirror * 0x20;  // 32bytes per mirror
        uint16_t spritePaletteBase = (uint16_t)0x3f11 + mirror * 0x20;
        for (int i = 0; i < 3; i++) {
            ppu.ppuMemory.colorPalette.unusedPaletteData[i] = 10 + i;
        }

        EXPECT_EQ(1, ppu.getByte(backgroundPaletteBase - 1));    // universal bkrnd
        // Background palette
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                uint16_t res = ppu.getByte(backgroundPaletteBase + i * 4 + j);
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
                uint16_t res1 = ppu.getByte(spritePaletteBase + i * 4 + j);
                if (j == 3) {
                    EXPECT_EQ(10 + i, res1);    // unused components
                } else {
                    EXPECT_EQ(100 + (i * 4 + j), res1);    // main palette
                }
            }
        }
    }
}


TEST_F(PPUMemoryMapperTest, testNameTableMapperVertical) {
    cart.mirroring = PPUMirroring::PPU_VERTICAL;
    setNameTableData();
    for (int table = 0; table < 4; table++) {
        uint16_t base = 0x2000 + table*0x400;
        // steps of 0x100 since the tests compile overly slow otherwise
        for (uint16_t i = 0; i < (uint16_t)0x400; i+= 0x100) {
            uint8_t res = ppu.getByte(base + i);
            if (i < 0x3c0) {
                if (table == 0 || table == 2) {
                    EXPECT_EQ(0xAB, res);
                } else {
                    EXPECT_EQ(0x12, res);
                }
            } else {
                if (table == 0 || table == 2) {
                    EXPECT_EQ(0xCD, res);
                } else {
                    EXPECT_EQ(0x34, res);
                }
            }
        }
    }
}

TEST_F(PPUMemoryMapperTest, testNameTableMapperHorizontal) {
    cart.mirroring = PPUMirroring::PPU_HORIZONTAL;
    setNameTableData();
    for (int table = 0; table < 4; table++) {
        uint16_t base = 0x2000 + table * 0x400;
        // steps of 0x100 since the tests compile overly slow otherwise
        for (uint16_t i = 0; i < (uint16_t)0x400; i += 0x100) {
            uint8_t res = ppu.getByte(base + i);
            if (table < 2) {
                if (i < 0x3c0) {
                    EXPECT_EQ(0xAB, res);
                } else {
                    EXPECT_EQ(0xCD, res);
                }
            } else {
                if (i < 0x3c0) {
                    EXPECT_EQ(0x12, res);
                } else {
                    EXPECT_EQ(0x34, res);
                }
            }
        }
    }
}