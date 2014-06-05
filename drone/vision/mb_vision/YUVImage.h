#ifndef MB_VISION_YUVIMAGE_H
#define MB_VISION_YUVIMAGE_H

#include "Image.h"
#include <stdint.h>

class YUVImage : public Image {
	public:
		YUVImage(int width, int height);

		virtual Pixel getPixel(int x, int y) const;
		virtual void setPixel(int x, int y, const Pixel &pixel);

	private:
		int getOffset(int x, int y) const;
};

#endif

