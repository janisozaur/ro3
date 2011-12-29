#ifndef THRESHOLDFILTER_H
#define THRESHOLDFILTER_H

#include <QImage>

class ThresholdFilter
{
public:
	ThresholdFilter(const int &threshold);
	QImage threshPassBelow(const QImage &img) const;
	QImage threshPassAbove(const QImage &img) const;
	void setThreshold(const int &threshold);
	QImage threshPassBand(const QImage &img, const int &lower, const int &upper) const;

private:
	int mThreshold;
};

#endif // THRESHOLDFILTER_H
