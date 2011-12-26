#include "GaussianFilter.h"
#include "EdgeDetector.h"

#include <QtCore/QCoreApplication>
#include <QImage>
#include <QStringList>

#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	const qreal size = 5;
	const qreal stdDev = GaussianFilter::devForSize(size);
	qDebug() << "stddev for size" << size << "equals" << stdDev;
	GaussianFilter gf(size, stdDev);
	QImage img(a.arguments().at(1));
	QImage result = gf.gaussianBlur(img);
	result.save(a.arguments().at(1) + "_gaussian.png");
	EdgeDetector ed;
	QImage edges = ed.detect(img);
	edges.save(a.arguments().at(1) + "_edges.png");
	return 0;
}
