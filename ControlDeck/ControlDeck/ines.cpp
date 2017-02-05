#include "ines.h"
#include <stdio.h>
#include <cstring>
#include "common.h"
#ifdef _MSC_VER
// Disable warnings for fopen
#pragma warning(disable:4996)
#endif

namespace NES {
    Cartridge * loadINesFile(char * fname, Cartridge * cart) {
        FILE *fp = fopen(fname, "r");
        // TODO what kind of error handling should be done here?
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char * data = new char[fsize];
        fread(data, fsize, 1, fp);
        fclose(fp);

        INesHeader header;
        DBG_ASSERT(parseHeader(data, &header), "Unable to parse INES header data");

        DBG_ASSERT(transferMemoryBankDataFromRom(data, &header, cart), "Unable to build cartridge from header and file data");
        return cart;
    }

    MemoryManagementController * generateMMCFromINesMemoryMapperNumber(INesHeader *header) {
        MemoryManagementController *mmc = nullptr;
        switch (header->mapperNumber) {
        // NROM
        case 0:
            // can have 1 or two PRG rom banks and one CHG rom bank
            mmc = new NRom(header->numPrgRomBanks == 2);
            break;
        // MMC1 ASIC
        // case 1:
        // UXRom
        // case 2:
        default:
            DBG_ASSERT(false, "Unsupported mapper number identified: %d", header->mapperNumber);
        };

        return mmc;
    }

    bool transferMemoryBankDataFromRom(char *nesFileData, INesHeader *header, Cartridge *cart) {
        if (header->hasTrainer) {
            cart->trainer = new uint8_t[512];
            memcpy(cart->trainer, &nesFileData[16], 512);
        }

        cart->numPrgRamBanks = header->numPrgRomBanks;
        cart->prgRom = new PrgRom[header->numPrgRomBanks]();
        for (int i = 0; i < header->numPrgRomBanks; i++) {
            memcpy(&cart->prgRom[i], &nesFileData[16 + (i * prgRomBankSize)], prgRomBankSize);
        }

        cart->numChrRomBanks = header->numChrRomBanks;
        cart->chrRom = new ChrRom[header->numChrRomBanks]();
        size_t start = 16 + (header->numPrgRomBanks * prgRomBankSize);
        for (int i = 0; i < header->numChrRomBanks; i++) {
            memcpy(&cart->chrRom[i], &nesFileData[start + (i * chrRomBankSize)], chrRomBankSize);
        }

        cart->numPrgRamBanks = header->numPrgRomBanks;
        cart->prgRam = new PrgRam[header->numRamBanks]();
        // TODO PlayChoice arcade cabinet INST-ROM/PROM?

        if (header->hasBatteryBackedRam) {
            cart->batteryBackedRam = new uint8_t[batteryBackedRamSize]();
        }


        switch (header->mirroring) {
        case INESMirroring::INES_VERTICAL:
            cart->mirroring = PPUMirroring::PPU_VERTICAL;
            break;
        case INESMirroring::INES_HORIZONTAL:
            cart->mirroring = PPUMirroring::PPU_HORIZONTAL;
            break;
        case INESMirroring::INES_FOUR_SCREEN:
            cart->mirroring = PPUMirroring::PPU_FOUR_SCREEN;
            break;
        default:
            DBG_ASSERT(false, "Invalid mirroring mode specified.  No mapping from INESMirroring to PPUMirroring found for %d", header->mirroring);
        };

        if (!(cart->mmc = generateMMCFromINesMemoryMapperNumber(header))) {
            return false;
        }
        
        return true;
    }

    bool parseHeader(char *nesFileData, INesHeader * header) {
        if (nesFileData == nullptr || header == nullptr ||
            !(nesFileData[0] == 'N' && nesFileData[1] == 'E' && nesFileData[2] == 'S') ||
            nesFileData[3] != 0x1a) {
            return false;
        }


        header->numPrgRomBanks = nesFileData[4];
        header->numChrRomBanks = nesFileData[5];
        char romControlByte = nesFileData[6];
        header->mirroring = (INESMirroring)(romControlByte & 0x1);
        header->hasBatteryBackedRam = (romControlByte & 0x2) != 0;
        header->hasTrainer = (romControlByte & 0x4) != 0;
        if ((romControlByte & 0x8) != 0) {
            header->mirroring = INES_FOUR_SCREEN;
        }
        char romControlByte2 = nesFileData[7];
        // assert bits 0-3 are 0?

        // mapper number from byte 6 and 7
        header->mapperNumber = romControlByte2 & 0xf0;
        header->mapperNumber += ((romControlByte & 0xf0) >> 4);

        if (nesFileData[8] == 0) {
            header->numRamBanks = 1;
        } else {
            header->numRamBanks = nesFileData[8];
        }
        // byte 9 always 0's.  TODO validate?
        return true;
    }

}