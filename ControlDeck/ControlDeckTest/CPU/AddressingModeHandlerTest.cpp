#include "gtest/gtest.h"
#include "CPUTestCommon.h"
#include "CPU/AddressingModeHandler.h"
using NES::AddressingMode;
using namespace NES::AddressingModeHandler;

class AddressingModeHandlerTest : public CPUTest {

};


TEST_F(AddressingModeHandlerTest, immediateAddressTest) {
    getSequentialMemory();
    uint16_t pc = registers.programCounter;
    handleAddressingMode(AddressingMode::Immediate, bus, registers, *memoryMapper);
    EXPECT_EQ(ram.ram[pc], bus.dataBus);
    EXPECT_EQ(pc + 1, registers.programCounter);
}

TEST_F(AddressingModeHandlerTest, immediateaddressReadTest) {
    bus.read = false;   // make sure read doesn't matter.
    getSequentialMemory();
    uint16_t pc = registers.programCounter;

    handleAddressingMode(AddressingMode::Immediate, bus, registers, *memoryMapper);
    EXPECT_EQ(ram.ram[pc], bus.dataBus);
    EXPECT_EQ(pc + 1, registers.programCounter);
}

TEST_F(AddressingModeHandlerTest, zeroPageAddressTest) {
    ram.ram[0] = 123;   // program counter
    ram.ram[123] = 0xFF;
    handleAddressingMode(AddressingMode::ZeroPage, bus, registers, *memoryMapper);
    EXPECT_EQ(0xff, bus.dataBus);

    bus.dataBus = 0;
    registers.programCounter = 300; // past byte size
    ram.ram[300] = 123;   // program counter
    handleAddressingMode(AddressingMode::ZeroPage, bus, registers, *memoryMapper);
    EXPECT_EQ(0xff, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, xZeroPageAddressTest) {
    registers.x = 2;
    ram.ram[0] = 123;   // program counter
    ram.ram[123] = 0xFF;
    ram.ram[123 + 2] = 0xEE;
    handleAddressingMode(AddressingMode::ZeroPageX, bus, registers, *memoryMapper);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, xZeroPageAddressWrapTest) {
    registers.x = 2;
    ram.ram[0] = 0xFF;   // program counter
    ram.ram[0xFF + 2] = 0xFF;   
    ram.ram[1] = 0xEE; // wrap around expected at 0xFF to 2
    handleAddressingMode(AddressingMode::ZeroPageX, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0001, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}


TEST_F(AddressingModeHandlerTest, yZeroPageAddressTest) {
    registers.y = 2;
    ram.ram[0] = 123;   // program counter
    ram.ram[123] = 0xFF;
    ram.ram[123 + 2] = 0xEE;
    handleAddressingMode(AddressingMode::ZeroPageY, bus, registers, *memoryMapper);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, yZeroPageAddressWrapTest) {
    registers.y = 2;
    ram.ram[0] = 0xFF;   // program counter
    ram.ram[0xFF + 2] = 0xFF;
    ram.ram[1] = 0xEE; // wrap around expected at 0xFF to 2
    handleAddressingMode(AddressingMode::ZeroPageY, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0001, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, absoluteAddressTest) {
    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0xDD;
    handleAddressingMode(AddressingMode::Absolute, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0123, bus.addressBus);
    EXPECT_EQ(0xDD, bus.dataBus);
}


TEST_F(AddressingModeHandlerTest, absoluteAddressXTest) {
    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0xDD;
    ram.ram[0x0125] = 0xEE;
    registers.x = 2;
    handleAddressingMode(AddressingMode::AbsoluteX, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0125, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, absoluteAddressYTest) {
    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0xDD;
    ram.ram[0x0125] = 0xEE;
    registers.y = 2;
    handleAddressingMode(AddressingMode::AbsoluteY, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0125, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, indirectAddressTest) {
    ram.ram[0] = 0x23;
    ram.ram[1] = 0x01;
    ram.ram[0x0123] = 0x12;
    ram.ram[0x12] = 0xEE;

    handleAddressingMode(AddressingMode::Indirect, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0012, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, xIndexedIndirectTest) {
    ram.ram[0] = 0x23;
    ram.ram[0x25] = 0x23;
    ram.ram[0x26] = 0x01;
    ram.ram[0x0123] = 0xEE;
    registers.x = 2;

    handleAddressingMode(AddressingMode::XIndexedIndirect, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0123, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}

TEST_F(AddressingModeHandlerTest, indirectYIndexedTest) {
    ram.ram[0] = 0x23;
    ram.ram[0x23] = 0x23;
    ram.ram[0x24] = 0x01;
    ram.ram[0x0124] = 0xEE;
    registers.y = 1;

    handleAddressingMode(AddressingMode::IndirectYIndexed, bus, registers, *memoryMapper);
    EXPECT_EQ(0x0124, bus.addressBus);
    EXPECT_EQ(0xEE, bus.dataBus);
}