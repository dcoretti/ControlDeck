#pragma once
#include <cstdint>
#include "common.h"
#include "CPU/SystemComponents.h"

namespace NES {
	// TODO re-evaluate where this belongs
	enum PPUMirroring {
		PPU_HORIZONTAL = 0,
		PPU_VERTICAL,
		PPU_FOUR_SCREEN	// indicated by ROM control bit 3 to override bit 0 to use four-screen mirroring
	};

	// 8kb 
	struct ChrRom {
		uint8_t rom[8192];
	};

	// 16kb rom bank for program data
	struct PrgRom {
		uint8_t rom[16384];
	};

	// Individual page of PRG ram.
	struct PrgRam {
		uint8_t ram[8192];
	};
	const size_t batteryBackedRamSize = 8192;
	class MemoryManagementController;
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

		void doMemoryOperation(SystemBus &bus, Cartridge &cart) override {
			DBG_ASSERT(bus.addressBus >= 0x8000, "Invalid bus address range for NROM: %d.", bus.addressBus);
			DBG_ASSERT(bus.read, "write operation detected to NROM.. not sure what to do here since there's no segfault");
			
			if (bus.addressBus >= 0xc000) {
				// Handles mirroring in the case that there is only one prg rom bank
				bus.dataBus = cart.prgRom[secondBankRomIndex].rom[bus.addressBus - 0xc000];
			} else {
				bus.dataBus = cart.prgRom[0].rom[bus.addressBus - 0x8000];
			}
		}

		// default to mirror first bank.  if 256 we switch this to reflect the presence of a second PRG bank
		int secondBankRomIndex{ 0 };
	};
}