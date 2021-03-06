#include <ControlDeck/Render.h>
#include <cstring>
#include <ControlDeck/common.h>

namespace NES {
    void RenderBuffer::putPixel(int x, int y, Pixel pixel) {
		// TODO when is this even going to happen?? Should this just be an assert?
        if (x > screen_w || y > screen_h) {
            return;
        }

		int index = 3 * screen_w * (screen_h - y - 1) + (3 * x);
		DBG_ASSERT(index >= 0, "index for putpixel was negative");
		DBG_ASSERT(index + 2 < 3 * screen_w * screen_h, "index somehow stepped out of bounds with 3bytesper pixel *screen dimensions"); // because I'm bad at math and seem to make this mistake often 
        renderBuffer[index] = pixel.r;
        renderBuffer[index + 1] = pixel.g;
        renderBuffer[index + 2] = pixel.b;
    }

    void RenderBuffer::clear() {
        memset(renderBuffer, 0, arrSizeof(renderBuffer));
    }

}