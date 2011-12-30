#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QPointF>

class Ellipse
{
public:
	Ellipse(const QPointF &center, const float &majorAxis,
			const float &minorAxis, const float &angle);
	QPointF center() const;
	float majorAxis() const;
	float minorAxis() const;
	float angle() const;

private:
	QPointF mCenter;
	float mMajorAxis;
	float mMinorAxis;
	float mAngle;
};

#endif // ELLIPSE_H
