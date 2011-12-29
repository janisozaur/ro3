#include "ThresholdFilter.h"

ThresholdFilter::ThresholdFilter(const int &threshold) :
	mThreshold(threshold)
{
}

QImage ThresholdFilter::threshPassBelow(const QImage &img) const
{
	QImage result = img;
	const int w = img.width();
	const int h = img.height();
	for (int y = 0; y < h; y++) {
		uchar *d = result.scanLine(y);
		for (int x = 0; x < w; x++) {
			if (d[x] > mThreshold) {
				d[x] = 255;
			} else {
				d[x] = 0;
			}
		}
	}
	return result;
}

QImage ThresholdFilter::threshPassAbove(const QImage &img) const
{
	QImage result = img;
	const int w = img.width();
	const int h = img.height();
	for (int y = 0; y < h; y++) {
		uchar *d = result.scanLine(y);
		for (int x = 0; x < w; x++) {
			if (d[x] > mThreshold) {
				d[x] = 0;
			} else {
				d[x] = 255;
			}
		}
	}
	return result;
}

QImage ThresholdFilter::threshPassBand(const QImage &img, const int &lower,
									   const int &upper) const
{
	QImage result = img;
	const int w = img.width();
	const int h = img.height();
	for (int y = 0; y < h; y++) {
		uchar *d = result.scanLine(y);
		for (int x = 0; x < w; x++) {
			if (d[x] < lower) {
				d[x] = 255;
			} else if (d[x] > upper) {
				d[x] = 255;
			} else {
				d[x] = 0;
			}
		}
	}
	return result;
}

void ThresholdFilter::setThreshold(const int &threshold)
{
	mThreshold = threshold;
}
