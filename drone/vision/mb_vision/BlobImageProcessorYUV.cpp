#include "BlobImageProcessorYUV.h"
#include <algorithm>
#include <iostream>

using namespace std;

BlobImageProcessorYUV::BlobImageProcessorYUV(int maxgapdist, int minsegmentsize, int minblobheight, bool debug, bool bluemarker)
: maxgapdist(maxgapdist), minsegmentsize(minsegmentsize), minblobheight(minblobheight), debug(debug), enable_bluemarker(bluemarker) { }

void BlobImageProcessorYUV::addColorModel(ColorModelYUV *model) {
	models.push_back(model);
}

#ifdef ENABLE_BLUEMARKER
void BlobImageProcessorYUV::processImage(/*const*/ Image &image) {
#else
void BlobImageProcessorYUV::processImage(const Image &image) {
#endif
	blobs.clear();

	int y;
	BlobList activeblobs;

	for (y=0; y<image.getHeight(); y++) { // for each row
		SegmentList segments = getSegments(y, image); // find all the segments

		for (SegmentList::iterator segment = segments.begin(); segment != segments.end(); ++segment) { // for each segment
			BlobList::iterator mainblob = matchSegment(*segment, activeblobs.begin(), activeblobs.end());

			if (mainblob != activeblobs.end()) { // if the segment matches a blob
				mainblob->h = y - mainblob->y; // extend its height down to the current row
				if (segment->start < mainblob->x) { // if the segment starts before the current blob
					mainblob->w += mainblob->x - segment->start; // extend the current blob
					mainblob->x = segment->start; // shift it to match the new segment
				}

				if (segment->end > mainblob->x + mainblob->w) // if the new segment ends after our current blob
					mainblob->w = segment->end - mainblob->x; // extend the current blob

				BlobList::iterator mergeblob = mainblob+1;
				while ((mergeblob = matchSegment(*segment, mergeblob, activeblobs.end())) != activeblobs.end()) { // continue looking for more blobs
					 // if we match another blob
					if (mainblob->x > mergeblob->x) { // and it starts before our current blob
						mainblob->w += mainblob->x - mergeblob->x; // preserve our mergeblob's end point
						mainblob->x = mergeblob->x; // move our current blob's start position
					}

					if (mainblob->x + mainblob->w < mergeblob->x + mergeblob->w) // and it is wider than our main blob
						mainblob->w = mergeblob->x + mergeblob->w - mainblob->x; // extend our main blob

					mergeblob = activeblobs.erase(mergeblob); // remove the merged blob
				}
			} else { // segment doesn't match a blob
				Blob newblob; // make a new one
				newblob.x = segment->start;
				newblob.w = segment->end - segment->start;
				newblob.y = y;
				newblob.h = 1;

				activeblobs.push_back(newblob);
			}
		}

		for (BlobList::iterator blob = activeblobs.begin(); blob != activeblobs.end();) { // go through all of the active blobs
			if (blob->y + blob->h + maxgapdist < y) { // if there is too large of a gap
				if (blob->h > minblobheight) // if they're tall enough
					blobs.push_back(*blob); // keep it, copy it to the main blob list
				blob = activeblobs.erase(blob); // remove them from the active blob list
			} else
				++blob;
		}
	}

	blobs.insert(blobs.end(), activeblobs.begin(), activeblobs.end()); // copy all blobs that reached the bottom of the screen to the main blob list

	if (debug) {
		for (BlobList::iterator i = blobs.begin(); i != blobs.end(); ++i) {
			cout << "Blob at (" << i->x << "," << i->y << ") size " << i->w << "x" << i->h << endl;
		}
	}
}

BlobImageProcessorYUV::BlobList::iterator BlobImageProcessorYUV::matchSegment(const Segment &segment, BlobList::iterator blob, BlobList::iterator end) const {
	for (; blob != end; ++blob) {
		if (segment.start <= blob->x) {
			if (segment.end > blob->x)
				break;
		} else {
			if (segment.start <= blob->x + blob->w)
				break;
		}
	}

	return blob;
}

#ifdef ENABLE_BLUEMARKER
BlobImageProcessorYUV::SegmentList BlobImageProcessorYUV::getSegments(int y, /*const*/ Image &image) const {
#else
BlobImageProcessorYUV::SegmentList BlobImageProcessorYUV::getSegments(int y, const Image &image) const {
#endif
	SegmentList segments;
	Segment cursegment = { -1, -1 };
	int gapcount = 0;
	int x;

	for (x=0; x<image.getWidth(); x++) {
		Pixel pixel = image.getPixel(x, y);
		if (checkModels(pixel)) { // if the pixel is valid

            #ifdef ENABLE_BLUEMARKER
            if(enable_bluemarker)
            {
                // Added by Jeremy
                //Pixel *bluemarker = new Pixel(0, 0, 255);
                Pixel *bluemarker = new Pixel(29, 255, 107); // YUV for blue
                image.setPixel(x, y, *bluemarker); // Mark pixel as bright blue for a match
                delete(bluemarker);
            }
            #endif

			if (cursegment.start == -1) { // if its the first pixel in a row
				cursegment.start = x; // start a new segment
				if (debug)
					cout << "New segment starting at (" << x << "," << y << ")" << endl;
			}

			gapcount = 0; // any valid pixel resets the gap counter
		} else if (cursegment.start >= 0) { // pixel not valid, but we're in a row
			if (++gapcount > maxgapdist) { // if we've exceeded the maximum gap count
				cursegment.end = x - maxgapdist; // set the end of the segment
				if (cursegment.end - cursegment.start >= minsegmentsize) { // if the segment is large enough
					segments.push_back(cursegment); // save it
					if (debug)
						cout << "segment saved, ends at " << cursegment.end << endl;
				} else {
					if (debug)
						cout << "segment rejected due to width (" << cursegment.end - cursegment.start << ")" << endl;
				}
				cursegment.start = -1;
			}
		}
	}

	// if a segment goes off the side of the image, but is wide enough
	if (cursegment.start > 0) {
		if (image.getWidth() - cursegment.start >= minsegmentsize) {
			cursegment.end = image.getWidth()-1-gapcount;
			segments.push_back(cursegment);
			if (debug)
				cout << "Saving segment, hit edge of screen" << endl;
		} else {
			if (debug)
				cout << "Segment hit edge of screen, rejected due to width (" << cursegment.end - cursegment.start << "(" << endl;
		}
	}

	return segments;
}

bool BlobImageProcessorYUV::checkModels(const Pixel &pixel) const {
	for (ColorModelList::const_iterator i = models.begin(); i != models.end(); ++i) {
		if ((*i)->checkPixel(pixel))
			return true;
	}

	return false;
}

