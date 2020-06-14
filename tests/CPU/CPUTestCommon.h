#pragma once
#include "gtest/gtest.h"
#include <ControlDeck/CPU\SystemComponents.h>
#include <ControlDeck/CPU/AddressingMode.h>
#include <ControlDeck/CPU/cpu2A03.h>
#include <ControlDeck/PPU/PPU2C02.h>

using NES::SystemBus;
using NES::SystemRam;
using NES::Registers;
using NES::DMAData;
using NES::Ppu2C02;
using NES::Cpu2a03;

class CPUTest : public testing::Test {
protected:
    static const uint8_t GOOD_BYTE = 0x12;

    virtual void SetUp() {
        cpu = Cpu2a03();
        cpu.ppu = &ppu;
    }

    virtual void TearDown() {
    }

    void getSequentialMemory() {
        for (int i = 0; i < SystemRam::systemRAMBytes; i++) {
            cpu.ram.ram[i] = i % 255;
        }
    }

    void getSystemRamWithMarkedOutOfRangeMemory() {
        for (int i = 0; i < NES::SystemRam::systemRAMBytes; i++) {
            cpu.ram.ram[i] = GOOD_BYTE;
        }
    }

    Cpu2a03 cpu;
    Ppu2C02 ppu;
};
