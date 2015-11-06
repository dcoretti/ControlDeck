#include "MemoryMapper.h"
namespace NES {

    void MemoryMapper::doMemoryOperation(SystemBus &systemBus) {
        if (systemBus.addressBus < 0x2000) {
            // 0x0800-0x0FFF  mirror 1
            // 0x1000-0x1FFF  mirror 2
            systemRamHandler(systemBus);
        }
        // I/O registers
        else if (systemBus.addressBus < 0x4020) {

        }
        // Expansion ROM
        else if (systemBus.addressBus < 0x6000) {

        }
        // SRAM
        else if (systemBus.addressBus < 0x8000) {

        }
        // up to $10000 is PRG-ROM
        else {

        }
    }

    // Read and write main CPU memory.  dataBusBuffer holds intermediate read value to be read
    // by instruction into register etc.
    void MemoryMapper::systemRamHandler(SystemBus &systemBus) {
        size_t actual = systemBus.addressBus % 800;
        if (systemBus.read) {
            systemBus.dataBus = systemRam->ram[actual];
        } else {
            systemRam->ram[actual] = systemBus.dataBus;
        }
    }

}