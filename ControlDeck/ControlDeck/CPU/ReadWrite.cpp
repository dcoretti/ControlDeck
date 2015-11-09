#pragma once

#include "ReadWrite.h"

namespace NES {
    namespace Cycle {
        namespace Util {
            // Single byte read from program counter
            void readDataFromProgramCounterSetup(SystemBus& systemBus, Registers &registers) {
                systemBus.addressBus = registers.programCounter;
                systemBus.read = true;
            }
        }
    }
}