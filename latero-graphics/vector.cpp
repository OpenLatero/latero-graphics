// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#include "vector.h"

namespace latero {
namespace graphics {

Point GetIntersectionOfLines(const Point &p1, const Vector &v1, const Point &p2, const Vector &v2)
{
	// http://ozviz.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/

	double x1 = p1.x;
	double y1 = p1.y;
	double x2 = p1.x + v1.x;
	double y2 = p1.y + v1.y;

	double x3 = p2.x;
	double y3 = p2.y;
	double x4 = p2.x + v2.x;
	double y4 = p2.y + v2.y;

	double ua = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
	ua /= (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);

	double ub = (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3);
	ub /= (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);

	return Point(x1 + ua*(x2-x1), y1 + ua*(y2-y1));
}

double GetDistanceToInfiniteLine(const Point &pos, const Point &origin, const Vector &dir)
{
	// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
	Vector V31 = pos - origin;
	float u = DotProduct(V31,dir) / dir.NormSquared();
	Vector p0 = dir*u;
	Vector Pi = origin + p0;
	Vector v = pos - Pi;
	return v.Norm();
}

double GetDistanceToLineSegment(const Point &pos, const Point &p1, const Point &p2)
{
	// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
	Vector V21 = p2 - p1;
	Vector V31 = pos - p1;
	float u = (V31.x*V21.x + V31.y*V21.y) / V21.NormSquared();
	Point p0 = V21*u;

	// intersection on the line
	Point Pi = p1 + p0;

	// find the closest point on the line
	Point P;
	if (u<0)	P = p1;
	else if (u>1)	P = p2;
	else		P = Pi;

	Point v = pos - P;
	return v.Norm();
}

double GetDistanceAlongLineSegment(const Point &pos, const Point &p1, const Point &p2)
{
	// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
	Point V21 = p2 - p1;
	Point V31 = pos - p1;
	float u = (V31.x*V21.x + V31.y*V21.y) / V21.NormSquared();
	Point p0 = V21*u;

	double rv = p0.Norm();
	if (u<0) rv *= -1;
	return rv;
}

void GetOrthoDistancesToLineSegment(const Point &pos, const Point &p1, const Point &p2, double &longitudinal, double &latitudinal)
{
	// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
	Vector V21 = p2 - p1;
	Vector V31 = pos - p1;
	float u = (V31.x*V21.x + V31.y*V21.y) / V21.NormSquared();
	Point p0 = V21*u;

	// intersection on the line
	Point Pi = p1 + p0;

	// find the closest point on the line
	Point P;
	if (u<0)	P = p1;
	else if (u>1)	P = p2;
	else		P = Pi;

	Point v = pos - P;
	latitudinal = v.Norm();

	longitudinal = p0.Norm();
	if (u<0) longitudinal *= -1;
}


bool IsInsidePolygon(Point pos, const std::vector<Point> &v)
{
	// http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
	bool rv = false;
	int i,j;
	for (i = 0, j = ((int)v.size())-1; i < (int)v.size(); j = i++)
	{
		if ((((v[i].y <= pos.y) && (pos.y < v[j].y)) ||
             	    ((v[j].y <= pos.y) && (pos.y < v[i].y))) &&
            	    (pos.x < (v[j].x - v[i].x) * (pos.y - v[i].y) / (v[j].y - v[i].y) + v[i].x))
          		rv = !rv;
      	}
      	return rv;
}

} // namespace graphics
} // namespace latero

