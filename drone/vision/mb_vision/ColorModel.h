#ifndef MB_VISION_COLORMODEL_H
#define MB_VISION_COLORMODEL_H

#include <string>
#include <stdint.h>
#include "HSVPixel.h"

class ColorModel {
	public:
		ColorModel(uint16_t lefthue, uint16_t righthue, uint8_t minsat, uint8_t minval, uint8_t maxsat=255, uint8_t maxval=255);
		ColorModel(int cbc_model);
		ColorModel();

		bool checkPixel(const HSVPixel &pixel) const;

	private:
		uint16_t lefthue, righthue;
		uint8_t minsat, maxsat;
		uint8_t minval, maxval;
};

#endif

