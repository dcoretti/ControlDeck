#include "gtest/gtest.h"

#include "CPUTestCommon.h"

using NES::ProcessorStatus;
using NES::InstructionFnPtr;
using NES::InstructionDispatcher;

class InstructionTest : public CPUTest {
protected:
    void testOnlyOneFlagSet(ProcessorStatus flag) {
        EXPECT_EQ((uint8_t)1 << flag, registers.statusRegister);
    }

    void assertDecrement(uint8_t &val, InstructionFnPtr instructionPtr) {
        val = 2;
        instructionPtr(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(0, registers.statusRegister);

        instructionPtr(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(0, val);
        testOnlyOneFlagSet(ProcessorStatus::ZeroFlag);

        instructionPtr(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(-1, (int8_t)val);
        testOnlyOneFlagSet(ProcessorStatus::NegativeFlag);


    }

    void assertDecrementWithDataBus(uint8_t &val, InstructionFnPtr instructionPtr) {
        bus.dataBus = 2;
        assertDecrement(val, instructionPtr);
    }

    void assertIncrement(uint8_t &val, InstructionFnPtr instructionPtr) {
        val = -2;
        instructionPtr(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(-1, (int8_t)val);
        testOnlyOneFlagSet(ProcessorStatus::NegativeFlag);

        instructionPtr(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
        EXPECT_EQ(0, val);
        testOnlyOneFlagSet(ProcessorStatus::ZeroFlag);

        instructionPtr(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
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
    assertDecrementWithDataBus(ram.ram[0x0123], &InstructionDispatcher::DEC);
}

TEST_F(InstructionTest, incTest) {
    bus.addressBus = 0x0123;
    assertIncrementWithDataBus(ram.ram[0x0123], &InstructionDispatcher::INC);
}

TEST_F(InstructionTest, dexTest) {
    assertDecrement(registers.x, &InstructionDispatcher::DEX);
}

TEST_F(InstructionTest, deyTest) {
    assertDecrement(registers.y, &InstructionDispatcher::DEY);
}

TEST_F(InstructionTest, inxTest) {
    assertIncrement(registers.x, &InstructionDispatcher::INX);
}

TEST_F(InstructionTest, inyTest) {
    assertIncrement(registers.y, &InstructionDispatcher::INY);
}

TEST_F(InstructionTest, taxTest) {
    registers.acc = 1;
    registers.x = 2;
    InstructionDispatcher::TAX(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(1, registers.x);
}

TEST_F(InstructionTest, tayTest) {
    registers.acc = 1;
    registers.y = 2;
    InstructionDispatcher::TAY(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(1, registers.y);
}

TEST_F(InstructionTest, txaTest) {
    registers.acc = 1;
    registers.x = 2;
    InstructionDispatcher::TXA(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(2, registers.acc);
}

TEST_F(InstructionTest, tyaTest) {
    registers.acc = 1;
    registers.y = 2;
    InstructionDispatcher::TYA(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(2, registers.acc);
}

TEST_F(InstructionTest, clcTest) {
    registers.setFlag(ProcessorStatus::CarryFlag);
    InstructionDispatcher::CLC(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    InstructionDispatcher::CLC(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}

TEST_F(InstructionTest, cldTest) {
    registers.setFlag(ProcessorStatus::DecimalMode);
    InstructionDispatcher::CLD(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    InstructionDispatcher::CLD(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}

TEST_F(InstructionTest, cliTest) {
    registers.setFlag(ProcessorStatus::InterruptDisable);
    InstructionDispatcher::CLI(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    InstructionDispatcher::CLI(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}
TEST_F(InstructionTest, clvTest) {
    registers.setFlag(ProcessorStatus::OverflowFlag);
    InstructionDispatcher::CLV(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);

    InstructionDispatcher::CLV(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    EXPECT_EQ(0, registers.statusRegister);
}
TEST_F(InstructionTest, secTest) {
    InstructionDispatcher::SEC(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    testOnlyOneFlagSet(ProcessorStatus::CarryFlag);
}

TEST_F(InstructionTest, sedTest) {
    InstructionDispatcher::SED(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    testOnlyOneFlagSet(ProcessorStatus::DecimalMode);
}

TEST_F(InstructionTest, seiTest) {
    InstructionDispatcher::SEI(InstructionDispatcher::opCodes[0xFF], bus, registers, *memoryMapper);
    testOnlyOneFlagSet(ProcessorStatus::InterruptDisable);
}