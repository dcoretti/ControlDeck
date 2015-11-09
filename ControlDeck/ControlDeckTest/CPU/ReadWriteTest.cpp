#include "gtest/gtest.h"

#include "CPU\ReadWrite.h"
using namespace NES;

SystemBus bus;
Registers registers;


TEST(ReadWriteTest, readTest) {
    registers.programCounter = 123;
    Cycle::Util::readDataFromProgramCounterSetup(bus, registers);
    EXPECT_EQ(true, bus.read);
    EXPECT_EQ(registers.programCounter, bus.addressBus);
}