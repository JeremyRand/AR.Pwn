#include "YUVImage.h"

//#include "../autoinclude_cbc.h"
#include <stdio.h>
#include <stdlib.h>

YUVImage::YUVImage(int width, int height)
: Image(width, height, width*height*6) { }

Pixel YUVImage::getPixel(int x, int y) const {
	int y1 = buf[2*y*2*width+2*x];
	int y2 = buf[2*y*2*width+2*x+1];
	int y3 = buf[(2*y+1)*2*width+2*x];
	int y4 = buf[(2*y+1)*2*width+2*x+1];
	int yavg = (y1 + y2 + y3 + y4) >> 2;
	int u = buf[4*height*width+y*width+x];
	int v = buf[5*height*width+y*width+x];
	return Pixel( (uint8_t) yavg, (uint8_t) u, (uint8_t) v);
}

void YUVImage::setPixel(int x, int y, const Pixel &pixel) {
	buf[2*y*2*width+2*x] = pixel.r;
	buf[2*y*2*width+2*x+1] = pixel.r;
	buf[(2*y+1)*2*width+2*x] = pixel.r;
	buf[(2*y+1)*2*width+2*x+1] = pixel.r;

	buf[4*height*width+y*width+x] = pixel.g;
	buf[5*height*width+y*width+x] = pixel.b;
}

int YUVImage::getOffset(int x, int y) const {
	return 0;
}

