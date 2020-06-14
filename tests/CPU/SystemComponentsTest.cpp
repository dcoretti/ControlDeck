#include <ControlDeck/CPU/SystemComponents.h>
#include "gtest/gtest.h"
#include <vector>

using namespace NES;
TEST(RegistersTest, pch) {
    Registers reg;
    reg.programCounter = 1 << 8;
    EXPECT_EQ(reg.pch(), 1);

    reg.programCounter = 1;
    EXPECT_EQ(reg.pch(), 0);
}

TEST(RegistersTest, pcl) {
    Registers reg;
    reg.programCounter = 1 << 8;
    EXPECT_EQ(reg.pcl(), 0);

    reg.programCounter = 1;
    EXPECT_EQ(reg.pcl(), 1);

    reg.programCounter = (1 << 8) + 1;
    EXPECT_EQ(reg.pcl(), 1);
}


TEST(RegistersTest, setPC) {
    Registers reg;
    uint8_t l = 0xab;
    uint8_t h = 0xcd;
    reg.setPcl(l);
    reg.setPch(h);
    EXPECT_EQ(0xcdab, reg.programCounter);

    EXPECT_EQ(l, reg.pcl());
    EXPECT_EQ(h, reg.pch());
}

std::vector<ProcessorStatus> statusFlags = {
    ProcessorStatus::CarryFlag,
    ProcessorStatus::ZeroFlag,
    ProcessorStatus::InterruptDisable,
    ProcessorStatus::DecimalMode,
    ProcessorStatus::OverflowFlag,
    ProcessorStatus::NegativeFlag,
};

TEST(RegistersTest, flagSetTest) {
    NES::Registers reg;    
    for (unsigned int i = 0; i < statusFlags.size(); i++) {
        reg.statusRegister = 1 << statusFlags[i];
        EXPECT_TRUE(reg.flagSet(statusFlags[i]));
    }
}


TEST(RegistersTest, setFlagTest) {
    NES::Registers reg;
    for (unsigned int i = 0; i < statusFlags.size(); i++) {
        reg.setFlag(statusFlags[i]);
        EXPECT_TRUE(reg.flagSet(statusFlags[i]));
    }
}

TEST(RegistersTest, clearFlagTest) {
    NES::Registers reg;
    reg.statusRegister = ~0;
    for (unsigned int i = 0; i < statusFlags.size(); i++) {
        EXPECT_TRUE(reg.flagSet(statusFlags[i]));
    }
    for (unsigned int i = 0; i < statusFlags.size(); i++) {
        EXPECT_TRUE(reg.flagSet(statusFlags[i]));
        reg.clearFlag(statusFlags[i]);
        EXPECT_FALSE(reg.flagSet(statusFlags[i]));
    }
}

TEST(RegistersTest, setFlagIfNegativeTest) {
    NES::Registers reg;
    reg.statusRegister = 0;
    EXPECT_FALSE(reg.flagSet(ProcessorStatus::NegativeFlag));
    reg.setFlagIfNegative(-1);
    EXPECT_TRUE(reg.flagSet(ProcessorStatus::NegativeFlag));

    reg.statusRegister = 0;
    reg.setFlagIfNegative(1);
    EXPECT_FALSE(reg.flagSet(ProcessorStatus::NegativeFlag));

    reg.statusRegister = 0;
    reg.setFlagIfNegative(0);
    EXPECT_FALSE(reg.flagSet(ProcessorStatus::NegativeFlag));

    reg.setFlag(ProcessorStatus::NegativeFlag);
    EXPECT_TRUE(reg.flagSet(ProcessorStatus::NegativeFlag));
    reg.setFlagIfNegative(-1);
    EXPECT_TRUE(reg.flagSet(ProcessorStatus::NegativeFlag));
}


TEST(SystemBusTest, setAddressBusTest) {
    SystemBus bus;
    bus.addressBus = 0;
    bus.setAddressBus(0, 0);
    EXPECT_EQ(0, bus.addressBus);

    bus.setAddressBus(0, 1);
    EXPECT_EQ(0x0100, bus.addressBus);

    bus.setAddressBus(1, 1);
    EXPECT_EQ(0x0101, bus.addressBus);
}

TEST(SystemBusTest, setAdlOnlyTest) {
    SystemBus bus;
    bus.addressBus = 0;

    bus.setAdlOnly(12);
    EXPECT_EQ(12, bus.addressBus);
}

TEST(SystemBusTest, setAdhOnlyTest) {
    SystemBus bus;
    bus.addressBus = 0;

    bus.setAdhOnly(1);
    EXPECT_EQ(0x0100, bus.addressBus);
}

TEST(SystemBusTest, setAdlTest) {
    SystemBus bus;
    bus.addressBus = 1 << 8;

    bus.setAdl (2);
    EXPECT_EQ(0x0102, bus.addressBus);
}

TEST(SystemBusTest, setAdhTest) {
    SystemBus bus;
    bus.addressBus = 2;

    bus.setAdh(1);
    EXPECT_EQ(0x0102, bus.addressBus);
}
