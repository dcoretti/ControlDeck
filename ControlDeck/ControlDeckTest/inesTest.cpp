#include "gtest/gtest.h"
#include "ines.h"
using namespace NES;
TEST(INES_Test, testBasicHeader) {
	INesHeader header;
	// Super Mario Bros header
	char fileData[] = {
		0x4e, 0x45, 0x53, 0x1a,
		0x02, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00 };

	EXPECT_EQ(true, parseHeader(fileData, &header));
	EXPECT_EQ(false, header.hasBatteryBackedRam);
	EXPECT_EQ(false, header.hasTrainer);
	EXPECT_EQ(0, header.mapperNumber);
	EXPECT_EQ(2, header.numPrgRomBanks);
	EXPECT_EQ(1, header.numChrRomBanks);
	EXPECT_EQ(INESMirroring::INES_VERTICAL, header.mirroring);
}


TEST(INES_Test, testBasicCartLoad) {
	INesHeader header;
	header.hasBatteryBackedRam = false;
	header.hasTrainer = false;
	header.mapperNumber = 0;
	header.mirroring = INESMirroring::INES_VERTICAL;
	header.numChrRomBanks = 1;
	header.numPrgRomBanks = 1;
	header.numRamBanks = 0;


	char fileData[16 + prgRomBankSize + chrRomBankSize];
	for (int i = 0; i < prgRomBankSize; i++) {
		fileData[16 + i] = 'a';
	}
	for (int i = 0; i < chrRomBankSize; i++) {
		fileData[16 + prgRomBankSize + i] = 'b';
	}

	Cartridge cart;
	EXPECT_TRUE(transferMemoryBankDataFromRom(fileData, &header, &cart));
	EXPECT_EQ(1, cart.numPrgRamBanks);
	EXPECT_EQ(1, cart.numChrRomBanks);
	EXPECT_EQ(1, cart.numPrgRamBanks);
	EXPECT_EQ(PPUMirroring::PPU_VERTICAL, cart.mirroring);
	EXPECT_EQ(nullptr, cart.trainer);
	EXPECT_EQ(nullptr, cart.batteryBackedRam);
	for (int i = 0; i < prgRomBankSize; i++) {
		if (cart.prgRom[0].rom[i] != 'a') {
			FAIL();
		}
	}
	for (int i = 0; i < chrRomBankSize; i++) {
		if (cart.chrRom[0].rom[i] != 'b') {
			FAIL();
		}
	}
}


TEST(INES_Test, testMMCCreation) {
	INesHeader header;
	header.hasBatteryBackedRam = false;
	header.hasTrainer = false;
	header.mapperNumber = 0;
	header.mirroring = INESMirroring::INES_VERTICAL;
	header.numChrRomBanks = 1;
	header.numPrgRomBanks = 1;
	header.numRamBanks = 0;

	char fileData[16 + prgRomBankSize + chrRomBankSize];
	fileData[16] = 'a';
	fileData[17] = 'b';

	Cartridge cart;
	EXPECT_TRUE(transferMemoryBankDataFromRom(fileData, &header, &cart));

	SystemBus bus;
	bus.addressBus = 0x8000;
	bus.read = true;
	cart.mmc->doMemoryOperation(bus, cart);
	EXPECT_EQ('a', bus.dataBus);
	bus.addressBus = 0x8001;
	cart.mmc->doMemoryOperation(bus, cart);
	EXPECT_EQ('b', bus.dataBus);

	// mirroring of single bank.
	bus.addressBus = 0xc000;
	cart.mmc->doMemoryOperation(bus, cart);
	EXPECT_EQ('a', bus.dataBus);
}

