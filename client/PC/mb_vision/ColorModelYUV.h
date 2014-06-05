#ifndef MB_VISION_COLORMODELYUV_H
#define MB_VISION_COLORMODELYUV_H

#include <string>
#include <stdint.h>
#include "HSVPixel.h"

class ColorModelYUV {
	public:
		ColorModelYUV(uint8_t ymin, uint8_t ymax, uint8_t umin, uint8_t umax, uint8_t vmin, uint8_t vmax);
		ColorModelYUV();

		bool checkPixel(const Pixel &pixel) const;

	private:
		uint8_t miny, maxy;
		uint8_t minu, maxu;
		uint8_t minv, maxv;
};

#endif

