#include "DilationFilter.h"

DilationFilter::DilationFilter(const int &kernelRadius) :
	mKernelRadius(kernelRadius)
{
}

QImage DilationFilter::dilate(const QImage &img) const
{
	QImage result = img;
	const int w = img.width();
	const int h = img.height();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			const int h1start = qMax(0, y - mKernelRadius);
			const int h1end = qMin(h, y + mKernelRadius);
			const int w1start = qMax(0, x - mKernelRadius);
			const int w1end = qMin(w, x + mKernelRadius);
			bool set = false;
			for (int y1 = h1start; y1 < h1end && !set; y1++) {
				for (int x1 = w1start; x1 < w1end; x1++) {
					if (img.pixelIndex(x1, y1) > 0) {
						set = true;
						break;
					}
				}
			}
			if (set) {
				result.setPixel(x, y, 255);
			} else {
				result.setPixel(x, y, 0);
			}
		}
	}
	return result;
}
