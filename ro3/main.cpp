#include "GaussianFilter.h"
#include "EdgeDetector.h"
#include "EllipseExtractor.h"
#include "DilationFilter.h"
#include "ThresholdFilter.h"
#include "BinaryOpFilter.h"

#include <QtCore/QCoreApplication>
#include <QImage>
#include <QStringList>
#include <QElapsedTimer>
#include <QDateTime>

#include <QDebug>

void drawResults(QImage &result, const QList<Ellipse> &ellipses, const int dotSize, const QRgb color)
{
	for (int i = 0; i < ellipses.size(); i++) {
		const QPoint c = ellipses.at(i).center().toPoint();
		const int y1start = qMax(0, c.y() - dotSize);
		const int y1end = qMin(c.y() + dotSize, result.height());
		const int x1start = qMax(0, c.x() - dotSize);
		const int x1end = qMin(c.x() + dotSize, result.width());
		for (int y = y1start; y < y1end; y++) {
			for (int x = x1start; x < x1end; x++) {
				result.setPixel(x, y, color);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	qsrand(QDateTime::currentDateTime().toTime_t());
	
	const qreal size = 5;
	const qreal stdDev = GaussianFilter::devForSize(size);
	qDebug() << "stddev for size" << size << "equals" << stdDev;
	GaussianFilter gf(size, stdDev);
	QImage img(a.arguments().at(1));
	Q_ASSERT_X(!img.isNull(), "loading image", "failed to load image");
	Q_ASSERT_X(img.format() == QImage::Format_Indexed8, "loading image", "image has more than 1 byte per channel");
	Q_ASSERT_X(!img.hasAlphaChannel(), "loading image", "image has alpha channel");
	Q_ASSERT_X(img.isGrayscale(), "loading image", "image is not grayscale");
	QImage gaussed = gf.gaussianBlur(img);
	ThresholdFilter tf(120);
	EdgeDetector ed;
	QImage edges = ed.detect(img);
	QImage grapesRed = tf.threshPassBelow(img);
	QImage edgesRed = ed.detect(grapesRed);
	DilationFilter df(2);
	tf.setThreshold(70);
	QImage edgesRedThreshed = tf.threshPassBelow(edgesRed);
	QImage dilated = df.dilate(df.dilate(edgesRed));
	QImage greenGrapesThreshed = tf.threshPassBand(img, 120, 200);
	QImage edgesGreen = ed.detect(greenGrapesThreshed);
	tf.setThreshold(45);
	QImage edgesGreenThreshed = tf.threshPassBelow(edgesGreen);
	dilated.invertPixels();
	BinaryOpFilter bof;
	QImage greenEdgesThreshedAnded = bof.imageAnd(dilated, edgesGreenThreshed);
#ifdef SAVE_IMAGES
	gaussed.save(a.arguments().at(1) + "_gaussian.png");
	edges.save(a.arguments().at(1) + "_edges_all.png");
	grapesRed.save(a.arguments().at(1) + "_threshed_above_120.png");
	edgesRed.save(a.arguments().at(1) + "_edges_red.png");
	edgesRedThreshed.save(a.arguments().at(1) + "_edges_red_threshed_65.png");
	dilated.save(a.arguments().at(1) + "_edges_red_threshed_dilated.png");
	greenGrapesThreshed.save(a.arguments().at(1) + "_green_threshed.png");
	edgesGreen.save(a.arguments().at(1) + "_edges_green.png");
	edgesGreenThreshed.save(a.arguments().at(1) + "_edges_green_threshed.png");
	greenEdgesThreshedAnded.save(a.arguments().at(1) + "_greenEdgesThreshedAnded.png");
#endif
	EllipseExtractor ee;
	QList<Ellipse> reds, greens;
	QElapsedTimer et;
	et.start();
	reds = ee.extract(edgesRedThreshed, img, 0, 100);
	const int msecsRed = et.elapsed();
	qDebug() << "overall red extraction took" << msecsRed << "msecs";
	qsrand(QDateTime::currentDateTime().toTime_t());
	greens = ee.extract(greenEdgesThreshedAnded, img, 100, 200);
	const int msecsGreen = et.elapsed();
	qDebug() << "overall green extraction took" << msecsGreen << "msecs";
	QImage result(img.size(), QImage::Format_ARGB32_Premultiplied);
	const int dotSize = 10;
	drawResults(result, reds, dotSize, qRgb(255, 0, 0));
	drawResults(result, greens, dotSize, qRgb(0, 255, 0));
	result.save(a.arguments().at(1) + "_result.png");
	return 0;
}
