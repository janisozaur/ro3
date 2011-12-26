#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H

#include <QImage>
#include <QVector>

class GaussianFilter
{
public:
	GaussianFilter(const int &size, const float &dev);
	void init(const int &size, const float &dev);
	QImage gaussianBlur(const QImage &img) const;
	static qreal devForRadius(const qreal &radius);
	static qreal devForSize(const qreal &size);

private:
	QVector<qreal> mKernel;
	qreal mScale;
	int mHalfSize;
};

#endif // GAUSSIANFILTER_H
