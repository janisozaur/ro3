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
	QElapsedTimer et;
	et.start();
	ee.extract(edgesRedThreshed);
	const int msecsRed = et.elapsed();
	qDebug() << "overall red extraction took" << msecsRed << "msecs";
	qsrand(QDateTime::currentDateTime().toTime_t());
	ee.extract(greenEdgesThreshedAnded);
	const int msecsGreen = et.elapsed();
	qDebug() << "overall green extraction took" << msecsGreen << "msecs";
	return 0;
}
