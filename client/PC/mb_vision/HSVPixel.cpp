#include "HSVPixel.h"
#include <algorithm>

using namespace std;

template <typename T> static T max3(T a, T b, T c) {
	return max(max(a, b), c);
}

template <typename T> static T min3(T a, T b, T c) {
	return min(min(a, b), c);
}

HSVPixel::HSVPixel(const Pixel &pixel) {
	const uint8_t min = min3(pixel.r, pixel.g, pixel.b);
	const uint8_t max = max3(pixel.r, pixel.g, pixel.b);
	const uint8_t diff = max - min;
	
	v = max;
	if (v == 0) {
		s = 0;
		h = 0;
		return;
	}
	
	s = diff*255/max;
	if (s == 0) {
		h = 0;
		return;
	}
	
	if (max == pixel.r)
		h = (pixel.g - pixel.b)*60/diff;
	else if (max == pixel.g)
		h = 120 + (pixel.b - pixel.r)*60/diff;
	else
		h = 240 + (pixel.r - pixel.g)*60/diff;
}

