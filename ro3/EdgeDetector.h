#ifndef EDGEDETECTOR_H
#define EDGEDETECTOR_H

#include <QImage>

class EdgeDetector
{
public:
	EdgeDetector();
	QImage detect(const QImage &img) const;

};

#endif // EDGEDETECTOR_H
