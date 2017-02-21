#include "Render.h"
#include <cstring>
#include "common.h"

namespace NES {
    void RenderBuffer::putPixel(int x, int y, Pixel pixel) {
        if (x > screen_w || y > screen_h) {
            return;
        }

        int index = screen_w * x + y;
        renderBuffer[index] = pixel.r;
        renderBuffer[index + 1] = pixel.g;
        renderBuffer[index + 2] = pixel.b;
    }

    void RenderBuffer::clear() {
        memset(renderBuffer, 0, arrSizeof(renderBuffer));
    }

}