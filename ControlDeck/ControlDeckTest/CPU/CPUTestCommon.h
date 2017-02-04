#pragma once
#include "gtest/gtest.h"
#include "CPU\SystemComponents.h"
#include "CPU\MemoryMapper.h"
#include "CPU/AddressingMode.h"
#include "CPU\InstructionDispatcher.h"

using NES::SystemBus;
using NES::SystemRam;
using NES::Registers;
using NES::MemoryMapper;

class CPUTest : public testing::Test {
protected:
    virtual void SetUp() {
        memoryMapper = new MemoryMapper(&ram);
        bus.addressBus = 0;
        bus.dataBus = 0;
        bus.read = false;

        registers.acc = 0;
        registers.programCounter = 0;
        registers.stackPointer = 0;
        registers.statusRegister = 0;
        registers.x = 0;
        registers.y = 0;
    }

    virtual void TearDown() {
        delete memoryMapper;
    }

    void getSequentialMemory() {
        for (int i = 0; i < SystemRam::systemRAMBytes; i++) {
            ram.ram[i] = i % 255;
        }
    }

    SystemBus bus;
    SystemRam ram;
    Registers registers;

    MemoryMapper * memoryMapper;
};
