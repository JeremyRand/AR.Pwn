#ifndef MB_VISION_GRIDIMAGEPROCESSOR_H
#define MB_VISION_GRIDIMAGEPROCESSOR_H

#include "Image.h"
#include "ColorModel.h"
#include <vector>

class GridImageProcessor {
	public:
		GridImageProcessor(int gridw, int gridh);
		~GridImageProcessor();
		
		void addColorModel(ColorModel *model); // keeps a pointer internally
		void processImage(const Image &image);
		
		int getCount(int x, int y, ColorModel *colormodel) const;
	private:
		int gridw, gridh;
		int *model_counts;
		
		typedef std::vector<ColorModel *> ColorModelList;
		ColorModelList models;
};

#endif

