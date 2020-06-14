#include "gtest/gtest.h"
#include <ControlDeck/CPU/cpu2A03.h>
#include "CPUTestCommon.h"

using NES::Cpu2a03;

class CPU2A03Test : public CPUTest {
protected:
    virtual void SetUp() {
        CPUTest::SetUp();
        ppu.disabled = true;

    }

    virtual void TearDown() {
        CPUTest::TearDown();
    }
};

TEST_F(CPU2A03Test, simpleInstructionSanityTest) {
    cpu.ram.ram[0] = 0xe8;  // INX
    cpu.registers.x = 2;
    cpu.registers.programCounter = 0;

    cpu.processInstruction();
    EXPECT_EQ(cpu.systemBus.dataBus, cpu.ram.ram[0]);
    EXPECT_EQ(3, cpu.registers.x);
    EXPECT_EQ(1, cpu.registers.programCounter);
}

TEST_F(CPU2A03Test, simpleTwoInstructionSanityTest) {
    cpu.ram.ram[0] = 0xe8;  // INX
    cpu.ram.ram[1] = 0xe8;  // INX

    cpu.registers.x = 2;
    cpu.registers.programCounter = 0;

    cpu.processInstruction();
    EXPECT_EQ(cpu.systemBus.dataBus, cpu.ram.ram[0]);
    EXPECT_EQ(3, cpu.registers.x);
    EXPECT_EQ(1, cpu.registers.programCounter);
    cpu.processInstruction();
    EXPECT_EQ(cpu.systemBus.dataBus, cpu.ram.ram[1]);
    EXPECT_EQ(4, cpu.registers.x);
    EXPECT_EQ(2, cpu.registers.programCounter);
}

TEST_F(CPU2A03Test, testDMA) {
    for (int i = 0; i < 256; i++) {
        cpu.ram.ram[i] = i;
    }
    cpu.dmaData.activate(0x00);
    int byte = 0;
    printf("starting\n");
    for (int i = 0; i < (256 * 2 + 2); i+=2) {
        EXPECT_TRUE(cpu.dmaData.isActive) << "dma went inactive too early :(";
        cpu.processInstruction();
        cpu.processInstruction();
        // Verify the PPU register port contains the right info
        if (i > 1) {
            EXPECT_EQ(cpu.ram.ram[byte], ppu.ppuMemory.memoryMappedRegisters.oamData) << "Ram wasn't equal!";
            byte++;
        }
    }
    EXPECT_EQ(cpu.dmaData.bytesWritten, 256) << "didn't get 256";
    EXPECT_FALSE(cpu.dmaData.isActive) << "still active";
}

TEST_F(CPU2A03Test, testSystemMemWrap) {
    getSystemRamWithMarkedOutOfRangeMemory();
    for (uint16_t i = 0; i < 0x2000; i++) {
        EXPECT_EQ(GOOD_BYTE, cpu.readFromAddress(i));
    }
}