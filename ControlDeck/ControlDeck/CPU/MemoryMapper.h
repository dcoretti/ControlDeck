#pragma once
#include "SystemComponents.h"

namespace NES {
	/*
	Source :http://nesdev.com/NESDoc.pdf Appendix D for memory mapper functions
	$1000
	$c000	PRG-ROM (upper)
	$8000	PRG-ROM (lower)
	$6000	Save RAM
	$4020	Expansion ROM
	$4000	I/O Registers
	$2008
	$2000	IO Registers

	mirrors of ram...

	-----------------------

	$0200	Main RAM
	$0100	Stack
	$0000	Zero Page



	*/
	class MemoryMapper {
	public:
		MemoryMapper(SystemRam * systemRam) : systemRam(systemRam) {}

		void doMemoryOperation(SystemBus &systemBus);

	private:
		void systemRamHandler(SystemBus &systemBus);

		SystemRam * systemRam;
	};


}