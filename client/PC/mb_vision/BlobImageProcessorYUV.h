#ifndef MB_VISION_BLOBIMAGEPROCESSORYUV_H
#define MB_VISION_BLOBIMAGEPROCESSORYUV_H

#include "Image.h"
#include "ColorModelYUV.h"
#include <vector>

#define ENABLE_BLUEMARKER 1

struct Blob {
	int x, y, w, h;
};

class BlobImageProcessorYUV {
	public:
		BlobImageProcessorYUV(int maxgapdist, int minsegmentsize, int minblobheight, bool debug=false, bool bluemarker=false);

		void addColorModel(ColorModelYUV *model); // keeps a pointer internally

		#if ENABLE_BLUEMARKER
		void processImage(/*const*/ Image &image);
		#else
		void processImage(const Image &image);
		#endif

		typedef std::vector<Blob> BlobList;
		const BlobList &getBlobs() const { return blobs; }

	private:
        bool enable_bluemarker;

		int maxgapdist, minsegmentsize, minblobheight;
		bool debug;
		BlobList blobs;

		typedef std::vector<ColorModelYUV *> ColorModelList;
		ColorModelList models;

		// --- helper functions ---

		bool checkModels(const Pixel &pixel) const;

		struct Segment {
			int start, end;
		};
		typedef std::vector<Segment> SegmentList;
		#if ENABLE_BLUEMARKER
		SegmentList getSegments(int y, /*const*/ Image &image) const;
		#else
		SegmentList getSegments(int y, const Image &image) const;
		#endif

		BlobList::iterator matchSegment(const Segment &segment, BlobList::iterator begin, BlobList::iterator end) const;
};

#endif

