#ifndef MB_VISION_IMAGE_H
#define MB_VISION_IMAGE_H

#include "Pixel.h"
#include <stdint.h>

class Image {
	public:
		Image(int width, int height, int bufsize);
		virtual ~Image();

		virtual Pixel getPixel(int x, int y) const = 0;
		virtual void setPixel(int x, int y, const Pixel &pixel) = 0;

		int getWidth() const { return width; }
		int getHeight() const { return height; }
		uint8_t *getBuffer() { return buf; }
		void setBuffer(uint8_t *buffer) {buf = buffer; }

	protected:
		int width, height;
		uint8_t *buf;
};

#endif

