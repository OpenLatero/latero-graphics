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

#include "dotpattern.h"
#include "../ridge.h"
#include "../units.h"
#include "dotpatternwidget.h"

namespace latero {
namespace graphics { 

DotPattern::DotPattern(const latero::Tactograph *dev) :
	Pattern(dev), Dots(dev), minSpacing_(1), vel_(0)
{
}

Gtk::Widget *DotPattern::CreateWidget(PatternPtr ptr)
{
	DotPatternPtr gen = boost::dynamic_pointer_cast<DotPattern>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new DotPatternWidget(gen);
}

void DotPattern::AppendXML(XMLOutputNode &root) const
{
	AppendHeight(root);
	AppendVib(root);
	AppendProfile(root);
	root.AddChild("min_spacing", GetMinSpacing(), units::mm);

}

void DotPattern::LoadXML(const XMLInputNode &root)
{
	LoadHeight(root);
	LoadVib(root);
	LoadProfile(root);
	XMLInputNode node = root.GetChild("min_spacing");
	if (node) SetMinSpacing(node.GetFloat());
}



//////////////////////////////////////////////////////////////////////////

DottedLine::DottedLine(const latero::Tactograph *dev, const Point &p0, const Point &p1) :
	Pattern(dev), DotPattern(dev), p0_(p0), p1_(p1)
{
	UpdateDots();
}

void DottedLine::UpdateDots()
{
	const double min_dot_dist = GetDotRadius()*2 + GetMinSpacing();
	LATERO_GRAPHICS_LOCK;
	Point p0 = p0_;
	Point p1 = p1_;
	LATERO_GRAPHICS_UNLOCK;

	Vector delta = p1-p0;
	float d = delta.Norm();
	
	// n: number of dots on the line
	int n = floor((d/min_dot_dist)+1);

	delta = delta / (n-1);

	ClearPoints();
	latero::graphics::Point dot = p0;
	for (int j=0; j<n; ++j)
	{
		InsertPoint(dot);
		dot.x += delta.x;
		dot.y += delta.y;
	}

	// precompute some stuff...
	LATERO_GRAPHICS_GUARD; // optimize more?
	Vector v01 = p1 - p0;
	u01_ = v01.Unit(); //
	n01_ = u01_.Normal(); //
	dBetweenPoints_ = delta.Norm();
	roundingLength_ = v01.Norm() + dBetweenPoints_; //
	pRoundingMin_ = p0_ - u01_*dBetweenPoints_/2; //
	pRoundingMax_ = p1_ - u01_*dBetweenPoints_/2; //
	period_ = boost::posix_time::microseconds(fabs(roundingLength_ / vel_)*1E6); //
}

double DottedLine::DoRender_(const ActuatorState &state)
{
	if (vel_ == 0)	return Dots::DoRender_(state);

	ActuatorState s = state;

	double d = GetDistanceAlongLineSegment(state.pos, pRoundingMin_, pRoundingMax_);
	double w = GetDistanceToInfiniteLine(state.pos, p0_, u01_);
	if ((d<0)||(d>roundingLength_)||(w>dotRadius_)) return 0;

	// avoid abrupt changes on sides
	const double modSize = 1;
	double mod = 1;
	double modDist = dBetweenPoints_/2 - dotRadius_;
	if (d-modDist < modSize)
		mod = (d-modDist)/modSize;
	else if (d>roundingLength_-modDist-modSize)
		mod = 1 - (d - (roundingLength_-modDist-modSize))/modSize;
	mod = fmax(0,fmin(1,mod));

	double us = state.GetTimeElapsed().total_microseconds() % period_.total_microseconds();
	double offset = fmod(roundingLength_ + d - us*vel_/1E6,roundingLength_);
	s.pos = pRoundingMin_ + u01_*offset;

	s.pos = s.pos + n01_*w;

	return mod*Dots::DoRender_(s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

DottedCircle::DottedCircle(const latero::Tactograph *dev, const Point &c, double r) :
	Pattern(dev), DotPattern(dev), center_(c), radius_(r)
{
	printf("DottedCircle constructor\n");
	UpdateDots();
}

void DottedCircle::UpdateDots()
{
	ClearPoints();

	// min_dot_dist is the distance between centers in straight line
	const double min_dot_dist = GetDotRadius()*2 + GetMinSpacing();
	const double min_theta = 2 * asin(min_dot_dist/(2*radius_));
	int n = floor(2*M_PI/min_theta);
	for (int i=0; i<n; ++i)
	{
		float a = ((float)i/(float)n)*(2*M_PI);
		float r = radius_;
		InsertPoint(GetCenter() + Point(r*cos(a),r*sin(a)));
	}

	// precompute some stuff
	boost::posix_time::time_duration period = boost::posix_time::microseconds(fabs(2*M_PI*GetRadius() / GetVel())*1E6);
	printf("DottedCircle::UpdateDots: period = %f ms; n = %d\n", period.total_microseconds()/1000.0, n);
	SetPeriod(period);
}

double DottedCircle::DoRender_(const ActuatorState &state)
{
	if (vel_ == 0)	return Dots::DoRender_(state);
	ActuatorState s = state;
	double us = state.GetTimeElapsed().total_microseconds() % period_.total_microseconds();
	double offset = 2*M_PI * fmod(1 - (float)us/(float)period_.total_microseconds(), 1);
	if (vel_<0) offset *= -1;
	Vector v_cp = state.pos - center_;
	s.pos = center_ + v_cp.Rotated(offset);
	return Dots::DoRender_(s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

DottedPolygon::DottedPolygon(const latero::Tactograph *dev) :
	Pattern(dev), DotPattern(dev), invertCornerVib_(false)
{
	UpdateDots();
}

void DottedPolygon::UpdateDots()
{
	ClearPoints();
	std::vector<PrecompData> data;
	std::vector<Point> vertices = GetVertices();
	bool invertCornerVib = GetInvertCornerVib();
	double vel = GetVel();

	const double minDotDist = dotRadius_*2 + GetMinSpacing();
	int nVertex = vertices.size();
	for (int i=0; i<nVertex; ++i)
	{
		double min_dot_dist = minDotDist;

		latero::graphics::Point p0 = vertices[(nVertex+i-1)%nVertex];
		latero::graphics::Point p1 = vertices[i];
		latero::graphics::Point p2 = vertices[(i+1)%nVertex];
		latero::graphics::Point p3 = vertices[(i+2)%nVertex];
		
		// check effect of angles
		// Todo: conservative, doesn't take into account position of dots on the adjacent lines
		// -> simplifies handling of motion (?)
		double angle1 = AngleRad(p0-p1, p2-p1);
		if (angle1 < M_PI/2)
			 min_dot_dist = fmax(min_dot_dist, minDotDist/sin(angle1));

		double angle2 = AngleRad(p1-p2, p3-p2);
		if (angle2 < M_PI/2)
			 min_dot_dist = fmax(min_dot_dist, minDotDist/sin(angle2));

		Vector v12 = p2 - p1;
		float d = v12.Norm();;
	
		// n: number of dots on the line
		int n = floor((d/min_dot_dist)+1);

		latero::graphics::Point dot = p1;
		for (int j=0; j<n-1; ++j)
		{
			InsertPoint(dot, (j==0)&&invertCornerVib);
			dot.x += v12.x/(n-1);
			dot.y += v12.y/(n-1);
		}

		// precomputed stuff for motion...
		PrecompData datum;
		datum.intersectDist  = dotRadius_ / tan(angle1/2);
		datum.u01_ = v12.Unit(); 
		datum.n01_ = datum.u01_.Normal(); 
		datum.dBetweenPoints = d/(n-1);
		datum.roundingLength_ = v12.Norm() - datum.dBetweenPoints; 
		datum.pRoundingMin_ = p1 + datum.u01_*datum.dBetweenPoints/2;
		datum.pRoundingMax_ = p2 - datum.u01_*datum.dBetweenPoints*3./2.;
		datum.period_ = boost::posix_time::microseconds(((datum.roundingLength_ / vel)*1E6));
		data.push_back(datum);
	}

	LATERO_GRAPHICS_GUARD;
	data_ = data;
}


double DottedPolygon::DoRender_(const ActuatorState &state)
{
	if (vel_ == 0)	return Dots::DoRender_(state);
	if (data_.size() != vertices_.size()) return Dots::DoRender_(state);

	ActuatorState s = state;

	for (unsigned int i=0; i<vertices_.size(); ++i)
	{
		PrecompData &data = data_[i];
		PrecompData &next_data = data_[(i+1)%data_.size()];

		double d = GetDistanceAlongLineSegment(state.pos, data.pRoundingMin_, data.pRoundingMax_);
		double w = GetDistanceToInfiniteLine(state.pos, data.pRoundingMin_, data.u01_);

		if ((d>=0)&&(d<=data.roundingLength_)&&(w<=dotRadius_))
		{
			// avoid abrupt changes on sides
			const double modSize = 1;
			double modDist1 = fmax(0,data.intersectDist-data.dBetweenPoints/2);
			double modDist2 = fmax(0,next_data.intersectDist-data.dBetweenPoints/2);
			double mod = 1;
			if (d-modDist1 < modSize)
				mod = (d-modDist1)/modSize;
			else if (d>data.roundingLength_-modDist2-modSize)
				mod = 1 - (d - (data.roundingLength_-modDist2-modSize))/modSize;
			mod = fmax(0,fmin(1,mod));

			double us = state.GetTimeElapsed().total_microseconds() % data.period_.total_microseconds();
			double offset = fmod(data.roundingLength_ + d - us*vel_/1E6,data.roundingLength_);
			s.pos = data.pRoundingMin_ + data.u01_*offset;

			s.pos = s.pos + data.n01_*w;

			return mod*Dots::DoRender_(s);
		}
	}

	return Dots::DoRender_(state);
}

void DottedPolygon::SetVertices(const std::vector<Point> &points)
{
	LATERO_GRAPHICS_LOCK;
	vertices_=points;
	LATERO_GRAPHICS_UNLOCK;
 	UpdateDots();
	SetLastModified();
}

} // namespace graphics
} // namespace latero



