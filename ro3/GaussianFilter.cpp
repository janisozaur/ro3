#include "GaussianFilter.h"

#include <cmath>
#include <QDebug>

// http://homepages.inf.ed.ac.uk/rbf/HIPR2/gsmooth.htm
// http://www.librow.com/articles/article-9

GaussianFilter::GaussianFilter(const int &size, const float &dev)
{
	init(size, dev);
}

void GaussianFilter::init(const int &size, const float &dev)
{
	if (size % 2 == 0) {
		qCritical() << "gaussian kernel must be of odd size";
	}
	mKernel.resize(0);
	mKernel.reserve(size * size);
	mHalfSize = size / 2;
	mScale = 0;
	const qreal power = -qreal(1) / (2 * dev * dev);
	const qreal den = -power * M_1_PI;
	for (int y = -mHalfSize; y <= mHalfSize; y++) {
		for (int x = -mHalfSize; x <= mHalfSize; x++) {
			const qreal kernelValue = den * exp(power * (y * y + x * x));
			mKernel << kernelValue;
			mScale += kernelValue;
		}
	}
	mScale = qreal(1) / mScale;
}

QImage GaussianFilter::gaussianBlur(const QImage &img) const
{
	QImage result(img.size(), QImage::Format_Indexed8);
	{
		QVector<QRgb> colorTable;
		colorTable.reserve(256);
		for (int i = 0; i < 256; i++) {
			colorTable << qRgb(i, i, i);
		}
		result.setColorTable(colorTable);
	}
	const int kernelSize = mHalfSize * 2 + 1;
	for (int y = 0; y < img.height(); y++) {
		for (int x = 0; x < img.width(); x++) {
			qreal sum = 0;
			for (int ky = -mHalfSize; ky <= mHalfSize; ky++) {
				const int yTemp = y + ky;
				int yIdx = yTemp;
				if (yTemp >= img.height()) {
					yIdx = img.height() - (yTemp - img.height()) - 1;
				} else if (yTemp < 0) {
					yIdx = -yTemp;
				}
				const uchar *dy = img.constScanLine(yIdx);
				for (int kx = -mHalfSize; kx <= mHalfSize; kx++) {
					const int xTemp = x + kx;
					int xIdx = xTemp;
					if (xTemp >= img.width()) {
						xIdx = img.width() - (xTemp - img.width()) - 1;
					} else if (xTemp < 0) {
						xIdx = -xTemp;
					}
					sum += dy[xIdx] * mKernel.at((ky + mHalfSize) * kernelSize + kx + mHalfSize);
				}
			}
			sum *= mScale;
			const int iSum = sum; //qBound(0, sum, 255)
			result.setPixel(x, y, iSum);
		}
	}
	return result;
}

// http://git.gnome.org/browse/gimp/tree/plug-ins/common/blur-gauss.c?id=61f3a0d338d1f3fe2993eaade82bdfec03f6e30d#n936
qreal GaussianFilter::devForRadius(const qreal &radius)
{
	const qreal r = radius + 1;
	const qreal logs = 2 * log(qreal(1) / qreal(255));
	return sqrt(-(r * r) / (logs));
}

qreal GaussianFilter::devForSize(const qreal &size)
{
	const int iSize = size;
	if (iSize != size || iSize % 2 == 0) {
		qWarning() << "gaussian kernel size should be an odd integer, it is"
				   << size;
	}
	return devForRadius((size - 1) / 2);
}
