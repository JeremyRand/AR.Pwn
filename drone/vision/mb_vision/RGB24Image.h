#ifndef MB_VISION_RGBIMAGE_H
#define MB_VISION_RGBIMAGE_H

#include "Image.h"
#include <stdint.h>

class RGB24Image : public Image {
	public:
		RGB24Image(int width, int height);

		virtual Pixel getPixel(int x, int y) const;
		virtual void setPixel(int x, int y, const Pixel &pixel);

        int outputFB1();

	private:
		int getOffset(int x, int y) const;
};

#endif

