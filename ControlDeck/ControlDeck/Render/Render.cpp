#include "Render.h"
#include <cstring>
#include "../common.h"

namespace NES {
    void RenderBuffer::putPixel(uint8_t x, uint8_t y, Pixel pixel) {
        int index = screen_w * x + y;
        renderBuffer[index] = pixel.r;
        renderBuffer[index + 1] = pixel.g;
        renderBuffer[index + 2] = pixel.b;
    }

    void RenderBuffer::clear() {
        memset(renderBuffer, 0, arrSizeof(renderBuffer));
    }

}