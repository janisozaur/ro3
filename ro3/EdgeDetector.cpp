#include "EdgeDetector.h"
#include "GaussianFilter.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

EdgeDetector::EdgeDetector()
{
}

QImage EdgeDetector::detect(const QImage &img) const
{
	Q_ASSERT(img.format() == QImage::Format_Indexed8);
	QImage result(img.size(), QImage::Format_Indexed8);
	{
		QVector<QRgb> colorTable;
		colorTable.reserve(256);
		for (int i = 0; i < 256; i++) {
			colorTable << qRgb(i, i, i);
		}
		result.setColorTable(colorTable);
	}
	GaussianFilter gf1(7, GaussianFilter::devForSize(7));
	GaussianFilter gf2(3, GaussianFilter::devForSize(3));
	const QImage res1 = gf1.gaussianBlur(img);
	const QImage res2 = gf2.gaussianBlur(img);
	QVector<int> diff(res2.width() * res2.height());
	int *diffPtr = diff.data();
	int max = 0;
	const int w = res1.width();
	const int h = res1.height();
	for (int y = 0; y < h; y++) {
		const uchar *d1y = res1.constScanLine(y);
		const uchar *d2y = res2.constScanLine(y);
		for (int x = 0; x < w; x++) {
			int difference = d1y[x] - d2y[x];
			//difference = ABS(difference);
			difference = qBound(0, difference, 255);
			diffPtr[y * w + x] = difference;
			if (difference > max) {
				max = difference;
			}
		}
	}
	const qreal scale = (max == 0 ? 1 : qreal(255.0) / max);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			const int difference = diff.at(y * w + x) * scale;
			result.setPixel(x, y, difference);
		}
	}
	return result;
}
