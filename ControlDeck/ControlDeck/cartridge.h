#pragma once
#include <cstdint>
#include "common.h"
#include "CPU/SystemComponents.h"

namespace NES {
	const size_t prgRomBankSize = 16384;
	static const size_t prgRamBankSize = 8192;
	const size_t chrRomBankSize = 8192;
	static const size_t batteryBackedRamSize = 8192;

	// TODO re-evaluate where this belongs
	enum PPUMirroring {
		PPU_HORIZONTAL = 0,
		PPU_VERTICAL,
		PPU_FOUR_SCREEN	// indicated by ROM control bit 3 to override bit 0 to use four-screen mirroring
	};

	// 8kb 
	struct ChrRom {
		uint8_t rom[chrRomBankSize];
	};

	// 16kb rom bank for program data
	struct PrgRom {
		uint8_t rom[prgRomBankSize];
	};

	// Individual page of PRG ram.
	struct PrgRam {
		uint8_t ram[prgRamBankSize];
	};


	class MemoryManagementController;

	// Representation of NES Game Pak including ROM/RAM and MMC
	struct Cartridge {
		uint8_t numPrgRomBanks;
		uint8_t numChrRomBanks;
		uint8_t numPrgRamBanks;	// pages of ram - 1 at minimum

		ChrRom *chrRom;
		PrgRom *prgRom;
		PrgRam *prgRam;

		//uint8_t *batteryBackedRam; // 
		uint8_t *trainer{ nullptr };	// 512bytes if present
		PPUMirroring mirroring;

		uint8_t *batteryBackedRam{ nullptr };	// 8192 if present

		MemoryManagementController *mmc;

	};



	//// function pointer declaration for memory mapper cartridge handler.
	//typedef void  (*MmcHandler)(SystemBus &bus);
	// TODO can I avoid virtual here?
	class MemoryManagementController {
	public:
		virtual void doMemoryOperation(SystemBus &bus, Cartridge &cart) = 0;
		virtual uint8_t doPPUReadOperation(uint16_t address, Cartridge &cart) = 0;
	};

	/*
	Simplest MMC.  All banks fixed.
	https://wiki.nesdev.com/w/index.php/NROM
	*/
	class NRom : public MemoryManagementController {
	public:
		NRom(bool is256) {
			if (is256) {
				secondBankRomIndex = 1;
			}
		}
		
		// TODO move memory address adjustment constants to a common location (0x8000 being first bank, etc.)
		void doMemoryOperation(SystemBus &bus, Cartridge &cart) override {
			// No Save RAM so 0x4020-0x8000 are not valid
			DBG_ASSERT(bus.addressBus >= 0x8000, "Invalid bus address range for NROM: %d.", bus.addressBus);
			DBG_ASSERT(bus.read, "write operation detected to NROM.. not sure what to do here since there's no segfault");
			
			if (bus.addressBus >= 0xc000) {
				// Handles mirroring in the case that there is only one prg rom bank
				bus.dataBus = cart.prgRom[secondBankRomIndex].rom[bus.addressBus - 0xc000];
			} else {
				bus.dataBus = cart.prgRom[0].rom[bus.addressBus - 0x8000];
			}
		}

		uint8_t doPPUReadOperation(uint16_t address, Cartridge &cart) override {
			DBG_ASSERT(address < 0x2000, "expected PPU address below 0x2000 in NROM mapper but got %d", address);
			return cart.chrRom[0].rom[address];
		}

		// default to mirror first bank.  if 256 we switch this to reflect the presence of a second PRG bank
		int secondBankRomIndex{ 0 };
	};
}