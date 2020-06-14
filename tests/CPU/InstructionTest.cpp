#include "gtest/gtest.h"

#include "CPUTestCommon.h"

using NES::ProcessorStatus;
using NES::InstructionFnPtr;
using namespace NES::InstructionSet;

class InstructionTest : public CPUTest {
protected:
    void testOnlyOneFlagSet(ProcessorStatus flag) {
        EXPECT_EQ((uint8_t)1 << flag, cpu.registers.statusRegister);
    }

    void assertDecrement(uint8_t &val, InstructionFnPtr instructionPtr) {
        val = 2;
        instructionPtr(opCodes[0xFF], cpu);
        EXPECT_EQ(0, cpu.registers.statusRegister);

        instructionPtr(opCodes[0xFF], cpu);
        EXPECT_EQ(0, val);
        testOnlyOneFlagSet(ProcessorStatus::ZeroFlag);

        instructionPtr(opCodes[0xFF], cpu);
        EXPECT_EQ(-1, (int8_t)val);
        testOnlyOneFlagSet(ProcessorStatus::NegativeFlag);
    }

    void assertDecrementWithDataBus(uint8_t &val, InstructionFnPtr instructionPtr) {
        cpu.systemBus.dataBus = 2;
        assertDecrement(val, instructionPtr);
    }

    void assertIncrement(uint8_t &val, InstructionFnPtr instructionPtr) {
        val = -2;
        instructionPtr(opCodes[0xFF], cpu);
        EXPECT_EQ(-1, (int8_t)val);
        testOnlyOneFlagSet(ProcessorStatus::NegativeFlag);

        instructionPtr(opCodes[0xFF], cpu);
        EXPECT_EQ(0, val);
        testOnlyOneFlagSet(ProcessorStatus::ZeroFlag);

        instructionPtr(opCodes[0xFF], cpu);
        EXPECT_EQ(1, val);
        EXPECT_EQ(0, cpu.registers.statusRegister);
    }

    void assertIncrementWithDataBus(uint8_t &val, InstructionFnPtr instructionPtr) {
        cpu.systemBus.dataBus = -2;
        assertIncrement(val, instructionPtr);
    }
};

TEST_F(InstructionTest, decTest) {
    cpu.systemBus.addressBus = 0x0123;
    assertDecrementWithDataBus(cpu.ram.ram[0x0123], &DEC);
}

TEST_F(InstructionTest, incTest) {
    cpu.systemBus.addressBus = 0x0123;
    assertIncrementWithDataBus(cpu.ram.ram[0x0123], &INC);
}

TEST_F(InstructionTest, dexTest) {
    assertDecrement(cpu.registers.x, &DEX);
}

TEST_F(InstructionTest, deyTest) {
    assertDecrement(cpu.registers.y, &DEY);
}

TEST_F(InstructionTest, inxTest) {
    assertIncrement(cpu.registers.x, &INX);
}

TEST_F(InstructionTest, inyTest) {
    assertIncrement(cpu.registers.y, &INY);
}

TEST_F(InstructionTest, taxTest) {
    cpu.registers.acc = 1;
    cpu.registers.x = 2;
    TAX(opCodes[0xFF], cpu);
    EXPECT_EQ(1, cpu.registers.x);
}

TEST_F(InstructionTest, tayTest) {
    cpu.registers.acc = 1;
    cpu.registers.y = 2;
    TAY(opCodes[0xFF], cpu);
    EXPECT_EQ(1, cpu.registers.y);
}

TEST_F(InstructionTest, txaTest) {
    cpu.registers.acc = 1;
    cpu.registers.x = 2;
    TXA(opCodes[0xFF], cpu);
    EXPECT_EQ(2, cpu.registers.acc);
}

TEST_F(InstructionTest, tyaTest) {
    cpu.registers.acc = 1;
    cpu.registers.y = 2;
    TYA(opCodes[0xFF], cpu);
    EXPECT_EQ(2, cpu.registers.acc);
}

TEST_F(InstructionTest, clcTest) {
    cpu.registers.setFlag(ProcessorStatus::CarryFlag);
    CLC(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);

    CLC(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);
}

TEST_F(InstructionTest, cldTest) {
    cpu.registers.setFlag(ProcessorStatus::DecimalMode);
    CLD(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);

    CLD(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);
}

TEST_F(InstructionTest, cliTest) {
    cpu.registers.setFlag(ProcessorStatus::InterruptDisable);
    CLI(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);

    CLI(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);
}
TEST_F(InstructionTest, clvTest) {
    cpu.registers.setFlag(ProcessorStatus::OverflowFlag);
    CLV(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);

    CLV(opCodes[0xFF], cpu);
    EXPECT_EQ(0, cpu.registers.statusRegister);
}
TEST_F(InstructionTest, secTest) {
    SEC(opCodes[0xFF], cpu);
    testOnlyOneFlagSet(ProcessorStatus::CarryFlag);
}

TEST_F(InstructionTest, sedTest) {
    SED(opCodes[0xFF], cpu);
    testOnlyOneFlagSet(ProcessorStatus::DecimalMode);
}

TEST_F(InstructionTest, seiTest) {
    SEI(opCodes[0xFF], cpu);
    testOnlyOneFlagSet(ProcessorStatus::InterruptDisable);
}


TEST_F(InstructionTest, phpTest) {
    cpu.registers.statusRegister = 0x04;
    cpu.registers.stackPointer = 0xfd;
    PHP(opCodes[0x08], cpu);
    EXPECT_EQ(0x34, cpu.ram.ram[NES::stackBaseAddress + 0xfd]);
    EXPECT_EQ(0x04, cpu.registers.statusRegister);    // didn't get cleared
    EXPECT_EQ(0xfc, cpu.registers.stackPointer);
}

TEST_F(InstructionTest, plpTest) {
    cpu.registers.statusRegister = 0x00;
    cpu.registers.stackPointer = 0xfc;
    cpu.ram.ram[NES::stackBaseAddress + 0xfd] = 0xcf;
    PLP(opCodes[0x08], cpu);
    EXPECT_EQ(0xcf, cpu.ram.ram[NES::stackBaseAddress + 0xfd]);
    EXPECT_EQ(0xcf, cpu.registers.statusRegister);    
    EXPECT_EQ(0xfd, cpu.registers.stackPointer);
}