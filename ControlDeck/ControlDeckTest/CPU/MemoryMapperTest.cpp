#include "gtest/gtest.h"

#include "CPU\MemoryMapper.h"

static const uint8_t GOOD_BYTE = 0x12;
NES::SystemRam getSystemRamWithMarkedOutOfRangeMemory() {
    NES::SystemRam ram;
    for (int i = 0; i < NES::SystemRam::systemRAMBytes; i++) {
        ram.ram[i] = GOOD_BYTE;
    }

    return ram;
}

TEST(MemoryMapperTest, testSystemMem) {
    NES::SystemRam ram = getSystemRamWithMarkedOutOfRangeMemory();
    NES::MemoryMapper mapper(&ram);

    NES::SystemBus bus;

    for (uint16_t i = 0; i < 0x2000; i++) {
        bus.addressBus = i;
        bus.read = true;
        mapper.doMemoryOperation(bus);

        EXPECT_EQ(GOOD_BYTE, bus.dataBus);
    }
}