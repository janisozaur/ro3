#include "BinaryOpFilter.h"

BinaryOpFilter::BinaryOpFilter()
{
}

QImage BinaryOpFilter::imageAnd(const QImage &img1, const QImage &img2) const
{
	QImage result = img1;
	const int w = img1.width();
	const int h = img1.height();
	for (int y = 0; y < h; y++) {
		const uchar *d1 = img1.constScanLine(y);
		const uchar *d2 = img2.constScanLine(y);
		for (int x = 0; x < w; x++) {
			result.setPixel(x, y, d1[x] & d2[x]);
		}
	}
	return result;
}

QImage BinaryOpFilter::imageOr(const QImage &img1, const QImage &img2) const
{
	QImage result = img1;
	const int w = img1.width();
	const int h = img1.height();
	for (int y = 0; y < h; y++) {
		const uchar *d1 = img1.constScanLine(y);
		const uchar *d2 = img2.constScanLine(y);
		for (int x = 0; x < w; x++) {
			result.setPixel(x, y, d1[x] | d2[x]);
		}
	}
	return result;
}
