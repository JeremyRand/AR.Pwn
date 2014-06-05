#ifndef MB_VISION_HSVPIXEL_H
#define MB_VISION_HSVPIXEL_H

#include <stdint.h>
#include "Pixel.h"

struct HSVPixel {
	HSVPixel() : h(0), s(0), v(0) { }
	HSVPixel(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) { }
	HSVPixel(const Pixel &pixel);

	uint16_t h;
	uint8_t s, v;
};

#endif
