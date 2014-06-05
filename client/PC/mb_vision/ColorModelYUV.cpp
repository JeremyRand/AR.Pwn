#include "ColorModelYUV.h"

ColorModelYUV::ColorModelYUV(uint8_t ymin, uint8_t ymax, uint8_t umin, uint8_t umax, uint8_t vmin, uint8_t vmax)
: miny(ymin), maxy(ymax), minu(umin), maxu(umax), minv(vmin), maxv(vmax) {

    //printf("New color model initialized: %d, %d, %d, %d\n", lefthue, righthue, minsat, minval);
    }

ColorModelYUV::ColorModelYUV()
: miny(0), maxy(0), minu(0), maxu(0), minv(0), maxv(0) {

    //printf("New color model initialized: %d, %d, %d, %d\n", lefthue, righthue, minsat, minval);
    }

bool ColorModelYUV::checkPixel(const Pixel &pixel) const {

	if (pixel.r < miny || pixel.r > maxy)
		return false;

	if (pixel.g < minu || pixel.g > maxu)
		return false;

	return (pixel.b > minv && pixel.b < maxv);
}

