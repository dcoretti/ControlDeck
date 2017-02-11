#include "gtest/gtest.h"

#include "CPUTestCommon.h"

using NES::ProcessorStatus;
using NES::InstructionFnPtr;
using namespace NES::InstructionSet;

class InstructionTest : public CPUTest {
protected:
    void testOnlyOneFlagSet(ProcessorStatus flag) {
        EXPECT_EQ((uint8_t)1 << flag, registers.statusRegister);
    }

    void assertDecrement(uint8_t &val, InstructionFnPtr instructionPtr) {
        val = 2;
        instructionPtr(opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(0, registers.statusRegister);

        instructionPtr(opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(0, val);
        testOnlyOneFlagSet(ProcessorStatus::ZeroFlag);

        instructionPtr(opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(-1, (int8_t)val);
        testOnlyOneFlagSet(ProcessorStatus::NegativeFlag);
    }

    void assertDecrementWithDataBus(uint8_t &val, InstructionFnPtr instructionPtr) {
        bus.dataBus = 2;
        assertDecrement(val, instructionPtr);
    }

    void assertIncrement(uint8_t &val, InstructionFnPtr instructionPtr) {
        val = -2;
        instructionPtr(opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(-1, (int8_t)val);
        testOnlyOneFlagSet(ProcessorStatus::NegativeFlag);

        instructionPtr(opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(0, val);
        testOnlyOneFlagSet(ProcessorStatus::ZeroFlag);

        instructionPtr(opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(1, val);
        EXPECT_EQ(0, registers.statusRegister);
    }

    void assertIncrementWithDataBus(uint8_t &val, InstructionFnPtr instructionPtr) {
        bus.dataBus = -2;
        assertIncrement(val, instructionPtr);
    }
};

TEST_F(InstructionTest, decTest) {
    bus.addressBus = 0x0123;
    assertDecrementWithDataBus(ram.ram[0x0123], &DEC);
}

TEST_F(InstructionTest, incTest) {
    bus.addressBus = 0x0123;
    assertIncrementWithDataBus(ram.ram[0x0123], &INC);
}

TEST_F(InstructionTest, dexTest) {
    assertDecrement(registers.x, &DEX);
}

TEST_F(InstructionTest, deyTest) {
    assertDecrement(registers.y, &DEY);
}

TEST_F(InstructionTest, inxTest) {
    assertIncrement(registers.x, &INX);
}

TEST_F(InstructionTest, inyTest) {
    assertIncrement(registers.y, &INY);
}

TEST_F(InstructionTest, taxTest) {
    registers.acc = 1;
    registers.x = 2;
    TAX(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(1, registers.x);
}

TEST_F(InstructionTest, tayTest) {
    registers.acc = 1;
    registers.y = 2;
    TAY(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(1, registers.y);
}

TEST_F(InstructionTest, txaTest) {
    registers.acc = 1;
    registers.x = 2;
    TXA(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(2, registers.acc);
}

TEST_F(InstructionTest, tyaTest) {
    registers.acc = 1;
    registers.y = 2;
    TYA(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(2, registers.acc);
}

TEST_F(InstructionTest, clcTest) {
    registers.setFlag(ProcessorStatus::CarryFlag);
    CLC(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    CLC(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}

TEST_F(InstructionTest, cldTest) {
    registers.setFlag(ProcessorStatus::DecimalMode);
    CLD(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    CLD(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}

TEST_F(InstructionTest, cliTest) {
    registers.setFlag(ProcessorStatus::InterruptDisable);
    CLI(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    CLI(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}
TEST_F(InstructionTest, clvTest) {
    registers.setFlag(ProcessorStatus::OverflowFlag);
    CLV(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    CLV(opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}
TEST_F(InstructionTest, secTest) {
    SEC(opCodes[0xFF], bus, registers, *memoryMapper);
    testOnlyOneFlagSet(ProcessorStatus::CarryFlag);
}

TEST_F(InstructionTest, sedTest) {
    SED(opCodes[0xFF], bus, registers, *memoryMapper);
    testOnlyOneFlagSet(ProcessorStatus::DecimalMode);
}

TEST_F(InstructionTest, seiTest) {
    SEI(opCodes[0xFF], bus, registers, *memoryMapper);
    testOnlyOneFlagSet(ProcessorStatus::InterruptDisable);
}


TEST_F(InstructionTest, phpTest) {
    registers.statusRegister = 0x04;
    registers.stackPointer = 0xfd;
    PHP(opCodes[0x08], bus, registers, *memoryMapper);
    EXPECT_EQ(0x34, ram.ram[stackBaseAddress + 0xfd]);
    EXPECT_EQ(0x04, registers.statusRegister);    // didn't get cleared
    EXPECT_EQ(0xfc, registers.stackPointer);
}

TEST_F(InstructionTest, plpTest) {
    registers.statusRegister = 0x00;
    registers.stackPointer = 0xfc;
    ram.ram[stackBaseAddress + 0xfd] = 0xcf;
    PLP(opCodes[0x08], bus, registers, *memoryMapper);
    EXPECT_EQ(0xcf, ram.ram[stackBaseAddress + 0xfd]);
    EXPECT_EQ(0xcf, registers.statusRegister);    
    EXPECT_EQ(0xfd, registers.stackPointer);
}