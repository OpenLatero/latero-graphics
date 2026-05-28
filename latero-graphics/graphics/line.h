#pragma once

#include "linefwd.h"
#include "dotpatternfwd.h"
#include "pattern.h"
#include "stroke.h"

namespace latero::graphics {

class Line : public Pattern
{
public:
	static LinePtr Create(const latero::Tactograph *dev, Point p1=Point(0,0), Point p2=Point(100,100))
	{
		return LinePtr(new Line(dev,p1,p2));
	}

	static LinePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return LinePtr(new Line(dev,node));
	}

	virtual ~Line();
	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t);

	virtual void AppendXML(XMLOutputNode &root) const;
	virtual void LoadXML(const XMLInputNode &root);

	virtual std::string GetName();

	StrokePtr GetStroke() 		{ return stroke_; }
	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

	void SetStartPoint(const Point &p) { SetEndPoints(p,p2_); SetLastModified();  }
	void SetEndPoint(const Point &p) { SetEndPoints(p1_,p); SetLastModified(); }
	Point GetStartPoint() const { LATERO_GRAPHICS_GUARD; return p1_; }
	Point GetEndPoint() const { LATERO_GRAPHICS_GUARD; return p2_; }

	virtual bool UpdateVisible_(const State *state);

private:
	Line(const latero::Tactograph *dev, Point p1, Point p2);
	Line(const latero::Tactograph *dev, const XMLInputNode &node);

	void Precompute();

	void SetEndPoints(Point p1, Point p2);

	Point p1_, p2_;
	StrokePtr stroke_;
	double length_; 	// pre-computed from p1,p2
	Point unitVector_; 	// pre-computed from p1,p2

	DottedLinePtr dots_;	// dots rendering
};

} // namespace

