#ifndef MB_VISION_RGBPIXEL_H
#define MB_VISION_RGBPIXEL_H

#include <stdint.h>

struct Pixel {
	Pixel() : r(0), g(0), b(0) { }
	Pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) { }
	
	uint8_t r, g, b;
};

#endif
