#include <ControlDeck/nes.h>
#include <ControlDeck/ines.h>

namespace NES {
    // TODO clean up initialization and general memory management
    void initNes(char * nesFile, NesControlDeck &controlDeck) {
        loadINesFile(nesFile, &controlDeck.cart);

        controlDeck.ppu.cartridge = &controlDeck.cart;
        controlDeck.cpu.ppu = &controlDeck.ppu;
        controlDeck.cpu.cartridge = &controlDeck.cart;

        controlDeck.cpu.debug = true;
        controlDeck.cpu.setPowerUpState();


        controlDeck.cpu.debugOutputFile = fopen("debug.txt", "w+");
    }


    DebugState step(NesControlDeck &nes) {
        return nes.cpu.processInstruction();
    }

    DebugState nesLoop(NesControlDeck &nes, int numInstructions) {
        bool countDown = numInstructions > -1;
        DebugState dbgState;
        int instructionCounter = 0;
        while (instructionCounter < numInstructions) {
            dbgState = nes.cpu.processInstruction();
            if (countDown) {
                instructionCounter++;
            }

            if (instructionCounter % 100 == 0 ) {
                printf("i: %d\n", instructionCounter);
            }
        }

        return dbgState;
    }
}