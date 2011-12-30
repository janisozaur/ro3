#include "Ellipse.h"

Ellipse::Ellipse(const QPointF &center, const float &majorAxis,
				 const float &minorAxis, const float &angle) :
	mCenter(center),
	mMajorAxis(majorAxis),
	mMinorAxis(minorAxis),
	mAngle(angle)
{
}

QPointF Ellipse::center() const
{
	return mCenter;
}

float Ellipse::majorAxis() const
{
	return mMajorAxis;
}

float Ellipse::minorAxis() const
{
	return mMinorAxis;
}

float Ellipse::angle() const
{
	return mAngle;
}
