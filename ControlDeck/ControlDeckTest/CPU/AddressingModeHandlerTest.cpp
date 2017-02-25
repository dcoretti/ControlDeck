#include "gtest/gtest.h"
#include "CPUTestCommon.h"
#include "CPU/AddressingModeHandler.h"
using NES::AddressingMode;

class AddressingModeHandlerTest : public CPUTest {

};


TEST_F(AddressingModeHandlerTest, immediateAddressTest) {
    getSequentialMemory();
    uint16_t pc = cpu.registers.programCounter;
    cpu.handleAddressingMode(AddressingMode::Immediate);
    EXPECT_EQ(cpu.ram.ram[pc], cpu.systemBus.dataBus);
    EXPECT_EQ(pc + 1, cpu.registers.programCounter);
}

TEST_F(AddressingModeHandlerTest, immediateaddressReadTest) {
    cpu.systemBus.read = false;   // make sure read doesn't matter.
    getSequentialMemory();
    uint16_t pc = cpu.registers.programCounter;

    cpu.handleAddressingMode(AddressingMode::Immediate);
    EXPECT_EQ(cpu.ram.ram[pc], cpu.systemBus.dataBus);
    EXPECT_EQ(pc + 1, cpu.registers.programCounter);
}

TEST_F(AddressingModeHandlerTest, zeroPageAddressTest) {
    cpu.ram.ram[0] = 123;   // program counter
    cpu.ram.ram[123] = 0xFF;
    cpu.handleAddressingMode(AddressingMode::ZeroPage);
    EXPECT_EQ(0xff, cpu.systemBus.dataBus);

    cpu.systemBus.dataBus = 0;
    cpu.registers.programCounter = 300; // past byte size
    cpu.ram.ram[300] = 123;   // program counter
    cpu.handleAddressingMode(AddressingMode::ZeroPage);
    EXPECT_EQ(0xff, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, xZeroPageAddressTest) {
    cpu.registers.x = 2;
    cpu.ram.ram[0] = 123;   // program counter
    cpu.ram.ram[123] = 0xFF;
    cpu.ram.ram[123 + 2] = 0xEE;
    cpu.handleAddressingMode(AddressingMode::ZeroPageX);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, xZeroPageAddressWrapTest) {
    cpu.registers.x = 2;
    cpu.ram.ram[0] = 0xFF;   // program counter
    cpu.ram.ram[0xFF + 2] = 0xFF;   
    cpu.ram.ram[1] = 0xEE; // wrap around expected at 0xFF to 2
    cpu.handleAddressingMode(AddressingMode::ZeroPageX);
    EXPECT_EQ(0x0001, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}


TEST_F(AddressingModeHandlerTest, yZeroPageAddressTest) {
    cpu.registers.y = 2;
    cpu.ram.ram[0] = 123;   // program counter
    cpu.ram.ram[123] = 0xFF;
    cpu.ram.ram[123 + 2] = 0xEE;
    cpu.handleAddressingMode(AddressingMode::ZeroPageY);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, yZeroPageAddressWrapTest) {
    cpu.registers.y = 2;
    cpu.ram.ram[0] = 0xFF;   // program counter
    cpu.ram.ram[0xFF + 2] = 0xFF;
    cpu.ram.ram[1] = 0xEE; // wrap around expected at 0xFF to 2
    cpu.handleAddressingMode(AddressingMode::ZeroPageY);
    EXPECT_EQ(0x0001, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, absoluteAddressTest) {
    cpu.ram.ram[0] = 0x23;
    cpu.ram.ram[1] = 0x01;
    cpu.ram.ram[0x0123] = 0xDD;
    cpu.handleAddressingMode(AddressingMode::Absolute);
    EXPECT_EQ(0x0123, cpu.systemBus.addressBus);
    EXPECT_EQ(0xDD, cpu.systemBus.dataBus);
}


TEST_F(AddressingModeHandlerTest, absoluteAddressXTest) {
    cpu.ram.ram[0] = 0x23;
    cpu.ram.ram[1] = 0x01;
    cpu.ram.ram[0x0123] = 0xDD;
    cpu.ram.ram[0x0125] = 0xEE;
    cpu.registers.x = 2;
    cpu.handleAddressingMode(AddressingMode::AbsoluteX);
    EXPECT_EQ(0x0125, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, absoluteAddressYTest) {
    cpu.ram.ram[0] = 0x23;
    cpu.ram.ram[1] = 0x01;
    cpu.ram.ram[0x0123] = 0xDD;
    cpu.ram.ram[0x0125] = 0xEE;
    cpu.registers.y = 2;
    cpu.handleAddressingMode(AddressingMode::AbsoluteY);
    EXPECT_EQ(0x0125, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, indirectAddressTest) {
    cpu.ram.ram[0] = 0x23;
    cpu.ram.ram[1] = 0x01;
    cpu.ram.ram[0x0123] = 0x12;
    cpu.ram.ram[0x12] = 0xEE;

    cpu.handleAddressingMode(AddressingMode::Indirect);
    EXPECT_EQ(0x0012, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, xIndexedIndirectTest) {
    cpu.ram.ram[0] = 0x23;
    cpu.ram.ram[0x25] = 0x23;
    cpu.ram.ram[0x26] = 0x01;
    cpu.ram.ram[0x0123] = 0xEE;
    cpu.registers.x = 2;

    cpu.handleAddressingMode(AddressingMode::XIndexedIndirect);
    EXPECT_EQ(0x0123, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}

TEST_F(AddressingModeHandlerTest, indirectYIndexedTest) {
    cpu.ram.ram[0] = 0x23;
    cpu.ram.ram[0x23] = 0x23;
    cpu.ram.ram[0x24] = 0x01;
    cpu.ram.ram[0x0124] = 0xEE;
    cpu.registers.y = 1;

    cpu.handleAddressingMode(AddressingMode::IndirectYIndexed);
    EXPECT_EQ(0x0124, cpu.systemBus.addressBus);
    EXPECT_EQ(0xEE, cpu.systemBus.dataBus);
}