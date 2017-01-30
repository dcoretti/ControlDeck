#include "gtest/gtest.h"
#include "cartridge.h"
using namespace NES;

TEST(Cartridge_Test, testNRomSingleBank) {
	NRom nrom(false);

	Cartridge cart;
	cart.prgRom = new PrgRom[1]();
	cart.prgRom[0].rom[0] = 'a';
	cart.prgRom[0].rom[1] = 'b';


	SystemBus bus;
	bus.addressBus = 0x8000;
	bus.read = true;

	nrom.doMemoryOperation(bus, cart);
	EXPECT_EQ('a', bus.dataBus);
	bus.addressBus = 0x8001;
	nrom.doMemoryOperation(bus, cart);
	EXPECT_EQ('b', bus.dataBus);

	//// mirroring of single bank.
	bus.addressBus = 0xc000;
	nrom.doMemoryOperation(bus, cart);
	EXPECT_EQ('a', bus.dataBus);
}

TEST(Cartridge_Test, testNRomMultiBank) {
	NRom nrom(true);

	Cartridge cart;
	cart.prgRom = new PrgRom[2]();
	cart.prgRom[0].rom[0] = 'a';
	cart.prgRom[1].rom[0] = 'b';

	SystemBus bus;
	bus.addressBus = 0x8000;
	bus.read = true;

	nrom.doMemoryOperation(bus, cart);
	EXPECT_EQ('a', bus.dataBus);

	/// second bank not mirrored
	bus.addressBus = 0xc000;
	nrom.doMemoryOperation(bus, cart);
	EXPECT_EQ('b', bus.dataBus);
}
