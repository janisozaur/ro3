#ifndef ELLIPSEEXTRACTOR_H
#define ELLIPSEEXTRACTOR_H

#include "Ellipse.h"

#include <QList>
#include <QImage>

class EllipseExtractor
{
public:
	EllipseExtractor();
	QList<Ellipse> extract(const QImage &image) const;
};

#endif // ELLIPSEEXTRACTOR_H
