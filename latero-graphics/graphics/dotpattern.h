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

#ifndef LATERO_GRAPHICS_PLANAR_DOT_PATTERN_H
#define LATERO_GRAPHICS_PLANAR_DOT_PATTERN_H

#include "dotpatternfwd.h"
#include "dots.h"
#include "../vector.h"

namespace latero {
namespace graphics { 

class DotPattern : public Dots
{
public:
	virtual ~DotPattern() {};
	virtual void UpdateDots() = 0;

	virtual void AppendXML(XMLOutputNode &root) const;
	virtual void LoadXML(const XMLInputNode &root);

	void SetMinSpacing(double v) { LATERO_GRAPHICS_GUARD; minSpacing_=v; UpdateDots(); SetLastModified_(); }
	double GetMinSpacing() const { LATERO_GRAPHICS_GUARD; return minSpacing_; }

	void SetThickness(double v) { SetDotRadius(v/2); UpdateDots(); SetLastModified(); }
	double GetThickness() const { return 2*GetDotRadius(); }

	void SetVel(double v) { LATERO_GRAPHICS_GUARD; vel_=v; UpdateDots(); SetLastModified(); }
	double GetVel() const { LATERO_GRAPHICS_GUARD; return vel_; }

	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

protected:
	DotPattern(const latero::Tactograph *dev);
	double minSpacing_;	// minimum distance between dots
	double vel_;
};


class DottedLine : public DotPattern
{
public:

	static DottedLinePtr Create(const latero::Tactograph *dev, const Point &p0, const Point &p1)
	{
		return DottedLinePtr(new DottedLine(dev,p0,p1));
	}
	virtual ~DottedLine() {};

	virtual double DoRender_(const ActuatorState &state);

	virtual void UpdateDots();

	void SetEndPoints(const Point &p0, const Point &p1) 
	{
		LATERO_GRAPHICS_LOCK;
		p0_=p0; 
		p1_=p1; 
		SetLastModified_(); 
		LATERO_GRAPHICS_UNLOCK;
		UpdateDots();
	}


protected:
	DottedLine(const latero::Tactograph *dev, const Point &p0, const Point &p1);

	Point p0_, p1_;

	// precomputed stuff
	double dBetweenPoints_;
	Vector u01_, n01_;
	double roundingLength_;
	Point pRoundingMin_, pRoundingMax_;
	boost::posix_time::time_duration period_;
};


class DottedCircle : public DotPattern
{
public:

	static DottedCirclePtr Create(const latero::Tactograph *dev, const Point &c, double r)
	{
		return DottedCirclePtr(new DottedCircle(dev,c,r));
	}
	virtual ~DottedCircle() {};

	virtual double DoRender_(const ActuatorState &state);

	virtual void UpdateDots();

	void SetCenter(const Point &p)
	{
		LATERO_GRAPHICS_LOCK; 
		center_=p; 
		SetLastModified_(); 
		LATERO_GRAPHICS_UNLOCK; 
		UpdateDots();

	}
	Point GetCenter() { LATERO_GRAPHICS_GUARD; return center_; }

	void SetRadius(double p)
	{
		LATERO_GRAPHICS_LOCK;
		radius_=p;
		SetLastModified_();
		LATERO_GRAPHICS_UNLOCK;
		UpdateDots();
	}
	double GetRadius() { LATERO_GRAPHICS_GUARD; return radius_; }

protected:
	DottedCircle(const latero::Tactograph *dev, const Point &c, double r);

	void SetPeriod(boost::posix_time::time_duration period) { LATERO_GRAPHICS_GUARD; period_=period; }


	Point center_;
	double radius_;

	// precomputed stuff...
	boost::posix_time::time_duration period_;
};

class DottedPolygon : public DotPattern
{
public:

	static DottedPolygonPtr Create(const latero::Tactograph *dev)
	{
		return DottedPolygonPtr(new DottedPolygon(dev));
	}
	virtual ~DottedPolygon() {};

	virtual double DoRender_(const ActuatorState &state);

	virtual void UpdateDots();

	void SetVertices(const std::vector<Point> &points);
	std::vector<Point> GetVertices() { LATERO_GRAPHICS_GUARD; return vertices_; }

	void SetInvertCornerVib(bool v) { LATERO_GRAPHICS_GUARD; invertCornerVib_=v; UpdateDots(); SetLastModified(); }
	bool GetInvertCornerVib() const { LATERO_GRAPHICS_GUARD; return invertCornerVib_; }

protected:
	DottedPolygon(const latero::Tactograph *dev);

	std::vector<Point> vertices_;

	// precomputed stuff
	typedef struct {
		double intersectDist;
		Vector u01_, n01_;
		double roundingLength_;
		Point pRoundingMin_, pRoundingMax_;
		double dBetweenPoints;
		boost::posix_time::time_duration period_;
	} PrecompData;

	std::vector<PrecompData> data_;
	bool invertCornerVib_;
};

} // namespace graphics
} // namespace latero

#endif
