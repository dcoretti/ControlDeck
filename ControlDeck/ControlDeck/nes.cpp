#include "nes.h"
#include "ines.h"

namespace NES {
    // TODO clean up initialization and general memory management
    void initNes(char * nesFile, NesControlDeck &controlDeck) {
        controlDeck.memoryMapper.systemRam = &controlDeck.systemRam;
        loadINesFile(nesFile, &controlDeck.cart);
        controlDeck.memoryMapper.cartridge = &controlDeck.cart;


        controlDeck.cpu.registers = &controlDeck.registers;
        controlDeck.cpu.ram = &controlDeck.systemRam;
        controlDeck.cpu.systemBus = &controlDeck.systemBus;
        controlDeck.cpu.memoryMapper = &controlDeck.memoryMapper;
        controlDeck.cpu.memoryMapper->ppu = &controlDeck.ppu;
        controlDeck.cpu.setPowerUpState();
        controlDeck.cpu.dmaData = &controlDeck.dmaData;
        //controlDeck.cpu.reset();


        controlDeck.ppu.cartridge = &controlDeck.cart;
    }

    void nesLoop(NesControlDeck &nes, int numInstructions) {
        bool countDown = numInstructions > -1;
        while (!nes.cpu.powerDown && numInstructions != 0) {
            DebugState dbgState = nes.cpu.processInstruction();
            if (countDown) {
                numInstructions--;
            }
            if (dbgState.opCode != nullptr) {
                // This is absolutely wrong but doing this until I can move PPU tick to the instruction level
                uint8_t totalCycles = dbgState.opCode->cycles + dbgState.addressingCycles + dbgState.branchCycles;
                for (int i = 0; i < totalCycles; i++) {
                    nes.ppu.doPpuCycle();
                    nes.ppu.doPpuCycle();
                    nes.ppu.doPpuCycle();
                }
            }
        }


    }
}