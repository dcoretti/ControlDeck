#pragma once
#include "gtest/gtest.h"
#include "CPU\SystemComponents.h"
#include "CPU\MemoryMapper.h"
#include "CPU/AddressingMode.h"
#include "CPU\InstructionDispatcher.h"
#include "PPU/PPUComponents.h"

using NES::SystemBus;
using NES::SystemRam;
using NES::Registers;
using NES::MemoryMapper;
using NES::DMAData;
using NES::PPURegisters;

class CPUTest : public testing::Test {
protected:
    virtual void SetUp() {
        memoryMapper = new MemoryMapper(&ram, &ppuRegisters);
        bus.addressBus = 0;
        bus.dataBus = 0;
        bus.read = false;

        registers.acc = 0;
        registers.programCounter = 0;
        registers.stackPointer = 0;
        registers.statusRegister = 0;
        registers.x = 0;
        registers.y = 0;

        ppuRegisters = PPURegisters();
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
    DMAData dmaData;
    PPURegisters ppuRegisters;

    MemoryMapper * memoryMapper;
};
