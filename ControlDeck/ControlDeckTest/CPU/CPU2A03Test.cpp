#include "gtest/gtest.h"
#include "CPU/cpu2A03.h"
#include "CPUTestCommon.h"

using NES::Cpu2a03;

class CPU2A03Test : public CPUTest {
protected:
    virtual void SetUp() {
        CPUTest::SetUp();
        cpu = new Cpu2a03(memoryMapper, &ram, &bus, &registers, &dmaData);
    }

    virtual void TearDown() {
        CPUTest::TearDown();
        delete cpu;
    }

    Cpu2a03 *cpu;
};

TEST_F(CPU2A03Test, simpleInstructionSanityTest) {
    ram.ram[0] = 0xe8;  // INX
    registers.x = 2;
    registers.programCounter = 0;

    cpu->processInstruction();
    EXPECT_EQ(bus.dataBus, ram.ram[0]);
    EXPECT_EQ(3, registers.x);
    EXPECT_EQ(1, registers.programCounter);
}

TEST_F(CPU2A03Test, simpleTwoInstructionSanityTest) {
    ram.ram[0] = 0xe8;  // INX
    ram.ram[1] = 0xe8;  // INX

    registers.x = 2;
    registers.programCounter = 0;

    cpu->processInstruction();
    EXPECT_EQ(bus.dataBus, ram.ram[0]);
    EXPECT_EQ(3, registers.x);
    EXPECT_EQ(1, registers.programCounter);
    cpu->processInstruction();
    EXPECT_EQ(bus.dataBus, ram.ram[1]);
    EXPECT_EQ(4, registers.x);
    EXPECT_EQ(2, registers.programCounter);
}

TEST_F(CPU2A03Test, testDMA) {
    for (int i = 0; i < 256; i++) {
        ram.ram[i] = i;
    }
    dmaData.activate(0x00);

    int byte = 0;
    for (int i = 0; i < (256 * 2 + 2); i+=2) {//)
        EXPECT_TRUE(dmaData.isActive);
        cpu->processInstruction();
        cpu->processInstruction();

        // Verify the PPU register port contains the right info
        if (i > 1) {
            EXPECT_EQ(ram.ram[byte], ppuRegisters.oamData);
            byte++;
        }
    }
    EXPECT_EQ(dmaData.bytesWritten, 256);
    EXPECT_FALSE(dmaData.isActive);
}