#include "GridImageProcessor.h"
#include <algorithm>

using namespace std;

GridImageProcessor::GridImageProcessor(int gridw, int gridh)
: gridw(gridw), gridh(gridh), model_counts(NULL) { }

GridImageProcessor::~GridImageProcessor() {
	delete model_counts;
}

void GridImageProcessor::addColorModel(ColorModel *model) {
	models.push_back(model);
	
	if (model_counts) {
		delete model_counts;
		model_counts = 0;
	}
}

void GridImageProcessor::processImage(const Image &image) {
	int totcounts = gridw*gridh*models.size();
	if (!model_counts)
		model_counts = new int [totcounts];
	fill(model_counts, model_counts + totcounts, 0);
	
	int gridsizex = image.getWidth() / gridw;
	int gridsizey = image.getHeight() / gridh;
	
	int gx, gy, px, py;
	for (gx=0; gx<gridw; gx++) {
		for (gy=0; gy<gridh; gy++) {
			for (px=0; px<gridsizex; px++) {
				for (py=0; py<gridsizey; py++) {
					int *curcount = &model_counts[(gx + gy*gridw)*models.size()];				
					HSVPixel pixel = image.getPixel(gx*gridsizex + px, gy*gridsizey + py);
					
					for (ColorModelList::const_iterator i = models.begin(); i != models.end(); ++i) {
						if ((*i)->checkPixel(pixel))
							(*curcount)++;

						curcount++;
					}					
				}
			}
		}
	}
}

int GridImageProcessor::getCount(int x, int y, ColorModel *model) const {
	unsigned int modelnum=0;
	for (ColorModelList::const_iterator i = models.begin(); i != models.end(); ++i) {
		if (*i == model)
			break;
		modelnum++;
	}
	if (modelnum == models.size())
		return -1;
	return model_counts[(y*gridw + x)*models.size() + modelnum];
}

