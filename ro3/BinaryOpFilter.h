#ifndef BINARYOPFILTER_H
#define BINARYOPFILTER_H

#include <QImage>

class BinaryOpFilter
{
public:
	BinaryOpFilter();
	QImage imageOr(const QImage &img1, const QImage &img2) const;
	QImage imageAnd(const QImage &img1, const QImage &img2) const;
};

#endif // BINARYOPFILTER_H
