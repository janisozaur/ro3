#ifndef DILATIONFILTER_H
#define DILATIONFILTER_H

#include <QImage>

class DilationFilter
{
public:
	DilationFilter(const int &kernelRadius);
	QImage dilate(const QImage &img) const;

private:
	int mKernelRadius;
};

#endif // DILATIONFILTER_H
