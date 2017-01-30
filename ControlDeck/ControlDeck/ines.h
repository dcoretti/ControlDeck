#pragma once
#include <cstdint>
#include "cartridge.h"

namespace NES {
	// Assumes a 1:1
	enum INESMirroring {
		INES_HORIZONTAL = 0,
		INES_VERTICAL,
		INES_FOUR_SCREEN	// indicated by ROM control bit 3 to override bit 0 to use four-screen mirroring
	};

	enum INesHeaderVersion {
		INES_ARCHAIC,
		INES0_7,
		INES,
		NES2_0
	};

	// Header file for ines format (.nes)
	struct INesHeader {
		uint8_t numPrgRomBanks;
		uint8_t numChrRomBanks;

		// (control bit 0/3) ROM Control byte data
		INESMirroring mirroring;
		// (control bit 1) battery backed ram at $6000-$7fff (8kb)
		bool hasBatteryBackedRam;
		// (control bit 2) 512-byte trainer at $7000-$71ff
		bool hasTrainer;
		uint8_t mapperNumber;

		uint8_t numRamBanks;	// 1 by default unless overridden by byte 8
		// TODO add functionality for INES0.7 and above, which utilize byte7 of the header and beyond.
		// See spec at https://wiki.nesdev.com/w/index.php/INES
	};




	Cartridge * loadINesFile(char * fname);
	bool transferMemoryBankDataFromRom(char *nesFileData, INesHeader *header, Cartridge *cart);
	bool parseHeader(char *nesFileData, INesHeader * header);

}