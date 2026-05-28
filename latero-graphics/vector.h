#pragma once

#include "point.h"
#include <vector>

// TODO clean this up
// use this? http://www.boost.org/doc/libs/1_37_0/libs/numeric/ublas/doc/index.htm

namespace latero::graphics {

class Vector : public Point
{
public:
	Vector()
	{
	}

	Vector(const Point &p) :
		Point(p)
	{
	}

	virtual ~Vector() {}

	/** @return unit vector */
	inline Vector Unit() const
	{
		return (*this)/Norm();
	}

	/** towards positive angles */
	Vector Normal() const
	{
		Vector rv;
		rv.x = y;
		rv.y = -x;
		return rv;
	}
		

	// TODO: inverted?
	Vector Rotated(double rad) const
	{
		Vector rv;
		rv.x = x*cos(rad) - y*sin(rad);
		rv.y = y*cos(rad) + x*sin(rad);
		return rv;
	}
};

/**
 * @return the dot product of vectors a and b
 */
inline double DotProduct(const Vector &a, const Vector &b)
{
	return a.x*b.x + a.y*b.y;
}

/**
 * @return the angle between vectors a and b, between 0 and M_PI
 */
inline double AngleRad(const Vector &a, const Vector &b)
{
	double rv = acos(DotProduct(a,b) / (a.Norm()*b.Norm()));
	if (rv<0) rv += 2*M_PI;
	return rv;
}


/**
 * Get the minimum distance between a point an infinite line given by a point and a vector.
 */
double GetDistanceToInfiniteLine(const Point &pos, const Point &origin, const Vector &dir);

/**
 * Get the minimum distance between a point and a line segment given its two end points.
 */
double GetDistanceToLineSegment(const Point &pos, const Point &p1, const Point &p2);

/**
 * Get the longitudinal distance along a line segment relative to the point p1. If the position is
 * before p1, the distance will be negative.
 */
double GetDistanceAlongLineSegment(const Point &pos, const Point &p1, const Point &p2);

void GetOrthoDistancesToLineSegment(const Point &pos, const Point &p1, const Point &p2, double &longitudinal, double &latitudinal);

Point GetIntersectionOfLines(const Point &p1, const Vector &v1, const Point &p2, const Vector &v2);

bool IsInsidePolygon(Point pos, const std::vector<Point> &v);

} // namespace

