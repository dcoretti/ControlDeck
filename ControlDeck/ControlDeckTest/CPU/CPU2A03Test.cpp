#include "gtest/gtest.h"
#include "CPU/cpu2A03.h"
#include "CPUTestCommon.h"

using NES::Cpu2a03;

class CPU2A03Test : public CPUTest {
protected:
    virtual void SetUp() {
        CPUTest::SetUp();
        cpu = new Cpu2a03(memoryMapper, &ram, &bus, &registers);
    }

    Cpu2a03 *cpu;
};

TEST_F(CPU2A03Test, simpleInstructionTest) {
    ram.ram[0] = 0xe8;  // INX
    registers.x = 2;
    registers.programCounter = 0;

    cpu->processInstruction();
    EXPECT_EQ(bus.dataBus, ram.ram[0]);
    EXPECT_EQ(3, registers.x);
    EXPECT_EQ(1, registers.programCounter);
}