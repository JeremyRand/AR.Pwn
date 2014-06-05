#include "ColorModel.h"

#include <stdio.h>

ColorModel::ColorModel(uint16_t lefthue, uint16_t righthue, uint8_t minsat, uint8_t minval, uint8_t maxsat, uint8_t maxval)
: lefthue(lefthue), righthue(righthue), minsat(minsat), maxsat(maxsat), minval(minval), maxval(maxval) {

    printf("New color model initialized: %d, %d, %d, %d\n", lefthue, righthue, minsat, minval);
    }

ColorModel::ColorModel()
: lefthue(0), righthue(0), minsat(0), maxsat(0), minval(0), maxval(0) {

    printf("New color model initialized: %d, %d, %d, %d\n", lefthue, righthue, minsat, minval);
    }

#if 0
ColorModel::ColorModel(int cbc_model)
{
    int h_min,h_max,s,v;
    track_get_model_hsv(cbc_model,&h_min,&h_max,&s,&v);
    lefthue = h_min;
    righthue = h_max;
    minsat = s;
    minval = v;
    maxsat = maxval = 255;

    if(cbc_model == 0)
    {
        lefthue = 0;
        righthue = 60;
        minsat = 128;
        minval = 128;
    }
    else if(cbc_model == 2)
    {
        lefthue = 90;
        righthue = 165; // 180 worked, some false positives on the poster... 150 noworky
        minsat = 128;
        minval = 64;
    }

    printf("New color model initialized: %d, %d, %d, %d\n", lefthue, righthue, minsat, minval);
}
#endif

bool ColorModel::checkPixel(const HSVPixel &pixel) const {
	if (pixel.s < minsat || pixel.s > maxsat)
		return false;

	if (pixel.v < minval || pixel.v > maxval)
		return false;

	return (lefthue < righthue) ^ (pixel.h < lefthue || pixel.h > righthue);
}

