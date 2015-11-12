#include "gtest/gtest.h"

#include "CPUTestCommon.h"

class InstructionTest : public CPUTest {

};

TEST_F(InstructionTest, decTest) {
    bus.addressBus = 0x0123;
    bus.dataBus = 5;

    NES::InstructionDispatcher::DEC(NES::InstructionDispatcher::opCodes[0xc6], bus, registers, *memoryMapper);
    EXPECT_EQ(4, ram.ram[0x0123]);
}

