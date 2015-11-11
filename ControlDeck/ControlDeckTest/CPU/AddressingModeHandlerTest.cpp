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

TEST(AddressingModeHandlerTest, immediateAddressTest) {
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

TEST(AddressingModeHandlerTest, zeroPageAddressTest) {
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

TEST(AddressingModeHandlerTest, xZeroPageAddressTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    registers.x = 2;
    ram.ram[0] = 123;   // program counter
    ram.ram[123] = 0xFF;
    ram.ram[123 + 2] = 0xEE;
    addrHandler.handleAddressingMode(NES::AddressingMode::ZeroPageX, bus, registers, memoryMapper);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, xZeroPageAddressWrapTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    registers.x = 2;
    ram.ram[0] = 0xFF;   // program counter
    ram.ram[0xFF + 2] = 0xFF;   
    ram.ram[1] = 0xEE; // wrap around expected at 0xFF to 2
    addrHandler.handleAddressingMode(NES::AddressingMode::ZeroPageX, bus, registers, memoryMapper);
    EXPECT_EQ(0x0001, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}


TEST(AddressingModeHandlerTest, yZeroPageAddressTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    registers.y = 2;
    ram.ram[0] = 123;   // program counter
    ram.ram[123] = 0xFF;
    ram.ram[123 + 2] = 0xEE;
    addrHandler.handleAddressingMode(NES::AddressingMode::ZeroPageY, bus, registers, memoryMapper);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, yZeroPageAddressWrapTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    registers.y = 2;
    ram.ram[0] = 0xFF;   // program counter
    ram.ram[0xFF + 2] = 0xFF;
    ram.ram[1] = 0xEE; // wrap around expected at 0xFF to 2
    addrHandler.handleAddressingMode(NES::AddressingMode::ZeroPageY, bus, registers, memoryMapper);
    EXPECT_EQ(0x0001, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, absoluteAddressTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0xDD;
    addrHandler.handleAddressingMode(NES::AddressingMode::Absolute, bus, registers, memoryMapper);
    EXPECT_EQ(0x0123, bus.addressBus);
    EXPECT_EQ(0xDD, bus.dataBus);
}


TEST(AddressingModeHandlerTest, absoluteAddressXTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0xDD;
    ram.ram[0x0125] = 0xEE;
    registers.x = 2;
    addrHandler.handleAddressingMode(NES::AddressingMode::AbsoluteX, bus, registers, memoryMapper);
    EXPECT_EQ(0x0125, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, absoluteAddressYTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0xDD;
    ram.ram[0x0125] = 0xEE;
    registers.y = 2;
    addrHandler.handleAddressingMode(NES::AddressingMode::AbsoluteY, bus, registers, memoryMapper);
    EXPECT_EQ(0x0125, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, indirectAddressTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0x12;
    ram.ram[0x12] = 0xEE;

    addrHandler.handleAddressingMode(NES::AddressingMode::Indirect, bus, registers, memoryMapper);
    EXPECT_EQ(0x0012, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, xIndexedIndirectTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 0x23;
    ram.ram[0x25] = 0x23;
    ram.ram[0x26] = 0x01;
    ram.ram[0x0123] = 0xEE;
    registers.x = 2;

    addrHandler.handleAddressingMode(NES::AddressingMode::XIndexedIndirect, bus, registers, memoryMapper);
    EXPECT_EQ(0x0123, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST(AddressingModeHandlerTest, indirectYIndexedTest) {
    setup();
    NES::SystemRam ram;
    NES::MemoryMapper memoryMapper(&ram);
    NES::AddressingModeHandler addrHandler;

    ram.ram[0] = 0x23;
    ram.ram[0x23] = 0x23;
    ram.ram[0x24] = 0x01;
    ram.ram[0x0124] = 0xEE;
    registers.y = 1;

    addrHandler.handleAddressingMode(NES::AddressingMode::IndirectYIndexed, bus, registers, memoryMapper);
    EXPECT_EQ(0x0124, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}