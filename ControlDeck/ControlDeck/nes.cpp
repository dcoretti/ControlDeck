#include "nes.h"
#include "ines.h"

namespace NES {
    // TODO clean up initialization and general memory management
    void initNes(char * nesFile, NesControlDeck &controlDeck) {
        loadINesFile(nesFile, &controlDeck.cart);

        controlDeck.ppu.cartridge = &controlDeck.cart;
        controlDeck.cpu.ppu = &controlDeck.ppu;
        controlDeck.cpu.cartridge = &controlDeck.cart;

        controlDeck.cpu.debug = true;
        controlDeck.cpu.setPowerUpState();
    }

    void nesLoop(NesControlDeck &nes, int numInstructions) {
        bool countDown = numInstructions > -1;
        while (numInstructions != 0) {
            DebugState dbgState = nes.cpu.processInstruction();
            if (countDown) {
                numInstructions--;
            }

            /// not sure what to do about this...
            if (dbgState.opCode != nullptr) {
                // This is absolutely wrong but doing this until I can move PPU tick to the instruction level
                uint8_t totalCycles = dbgState.opCode->cycles + dbgState.addressingCycles + dbgState.branchCycles;
            }
        }


    }
}