#pragma once
#include <cstdint>
#include "PPU/ColorPalette.h"

namespace NES {
    const size_t screen_w = 256;
    const size_t screen_h = 240;

    struct RenderBuffer {
        void putPixel(int x, int y, Pixel pixel);
        void clear();
        uint8_t renderBuffer[3 * sizeof(uint8_t) * screen_w * screen_h]{ 0 };
    };

}