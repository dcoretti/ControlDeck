#include "gtest/gtest.h"

#include "CPU\AddressingMode.h"
#include "CPU\AddressingModeHandler.h"

namespace {
    NES::SystemBus bus;
    NES::Registers registers;
}
NES::SystemRam getSequentialMemory() {
    NES::SystemRam systemRam;
    for (int i = 0; i < NES::SystemRam::systemRAMBytes; i++) {
        systemRam.ram[i] = i % 255;
    }
    return systemRam;
}
void setup() {
    bus.addressBus = 0;
    bus.dataBus = 0;
    bus.read = false;
    
    registers.acc = 0;
    registers.programCounter = 0;
    registers.stackPointer = 0;
    registers.statusRegister = 0;
    registers.x = 0;
    registers.y = 0;
}

TEST(InstructionTest, immediateAddressTest) {
    setup();
    NES::SystemRam ram = getSequentialMemory();
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;
    
    (bus, registers, memoryMapper);
    EXPECT_EQ(ram.ram[registers.programCounter], bus.dataBus);

    setup();
    bus.read = false;   // make sure read doesn't matter.
    ram = getSequentialMemory();
    addrHandler.handleAddressingMode(NES::AddressingMode::Immediate, bus, registers, memoryMapper);
    EXPECT_EQ(ram.ram[registers.programCounter], bus.dataBus);
}

TEST(InstructionTest, zeroPageAddressTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 123;   // program counter
    ram.ram[123] = 0xFF;
    addrHandler.handleAddressingMode(NES::AddressingMode::ZeroPage, bus, registers, memoryMapper);
    EXPECT_EQ(0xff, bus.dataBus);

    bus.dataBus = 0;
    registers.programCounter = 300; // past byte size
    ram.ram[300] = 123;   // program counter
    addrHandler.handleAddressingMode(NES::AddressingMode::ZeroPage, bus, registers, memoryMapper);
    EXPECT_EQ(0xff, bus.dataBus);

}
