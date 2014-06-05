#include "Image.h"

Image::Image(int width, int height, int bufsize)
: width(width), height(height),
  buf(new uint8_t [bufsize]) { }
  
Image::~Image() {
	delete buf;
}

