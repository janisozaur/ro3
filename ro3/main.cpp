#include "GaussianFilter.h"
#include "EdgeDetector.h"
#include "EllipseExtractor.h"

#include <QtCore/QCoreApplication>
#include <QImage>
#include <QStringList>
#include <QElapsedTimer>

#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	const qreal size = 5;
	const qreal stdDev = GaussianFilter::devForSize(size);
	qDebug() << "stddev for size" << size << "equals" << stdDev;
	GaussianFilter gf(size, stdDev);
	QImage img(a.arguments().at(1));
	Q_ASSERT_X(!img.isNull(), "loading image", "failed to load image");
	Q_ASSERT_X(img.format() == QImage::Format_Indexed8, "loading image", "image has more than 1 byte per channel");
	Q_ASSERT_X(!img.hasAlphaChannel(), "loading image", "image has alpha channel");
	Q_ASSERT_X(img.isGrayscale(), "loading image", "image is not grayscale");
	QImage result = gf.gaussianBlur(img);
	result.save(a.arguments().at(1) + "_gaussian.png");
	EdgeDetector ed;
	QImage edges = ed.detect(img);
	edges.save(a.arguments().at(1) + "_edges.png");
	EllipseExtractor ee;
	QElapsedTimer et;
	et.start();
	ee.extract(edges);
	const int msecs = et.elapsed();
	qDebug() << "overall ellipse extraction took" << msecs << "msecs";
	return 0;
}
