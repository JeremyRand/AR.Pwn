#include "RGB24Image.h"

//#include "../autoinclude_cbc.h"
#include <stdio.h>
#include <stdlib.h>

RGB24Image::RGB24Image(int width, int height)
: Image(width, height, width*height*3) { }

Pixel RGB24Image::getPixel(int x, int y) const {
	const uint8_t *pix = buf + getOffset(x, y);
	return Pixel(pix[0], pix[1], pix[2]);
}

void RGB24Image::setPixel(int x, int y, const Pixel &pixel) {
	uint8_t *pix = buf + getOffset(x, y);
	pix[0] = pixel.r;
	pix[1] = pixel.g;
	pix[2] = pixel.b;
}

int RGB24Image::getOffset(int x, int y) const {
	return (x + y*width)*3;
}

int RGB24Image::outputFB1() {

    FILE * rgbOut = fopen("/tmp/drone_fb1.rgb", "w");

    unsigned short start = 0;
    unsigned short result = 0;
    unsigned short r, g, b;

    for(int img_y=0; img_y < 240; img_y++)
    {
        for(int img_x=0; img_x < 320; img_x++)
        {
            if(img_x >= width || img_x >= height)
            {
                result = 0;
            }
            else
            {
                r = buf[getOffset(img_x, img_y) + 0] >> 3;
				g = buf[getOffset(img_x, img_y) + 1] >> 2;
				b = buf[getOffset(img_x, img_y) + 2] >> 3;

				result = ( ( ( (r << 3) | (g >> 3) ) & 0xFF ) << 8 ) | ( ( ( (g << 5) | b ) ) & 0xFF );
            }

            fwrite (&result , 1 , 2 , rgbOut );
        }
    }

    fclose(rgbOut);

    system("cat /tmp/drone_fb1.rgb > /dev/fb1");

	return 0;
}

