#pragma once

#include <memory>
#include "closedpattern.h"
#include "stroke.h"
#include "dotpattern.h"

namespace latero::graphics {

typedef std::shared_ptr<class Circle> CirclePtr;

class Circle : public ClosedPattern
{
public:
	static CirclePtr Create(const latero::Tactograph *dev, Point center=Point(0,0), double radius=100)
	{
		return CirclePtr(new Circle(dev, center, radius));
	}

	static CirclePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return CirclePtr(new Circle(dev, node));
	}

	virtual void AppendXML(XMLOutputNode &root) const;
	virtual void LoadXML(const XMLInputNode &root);

	virtual ~Circle() {};
	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);
	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t);

	double RenderFilling_(double dLength, double dWidth, const ActuatorState &state);

	/** 
	 * Compute the longitudinal (along the length) and latitudinal (along the width) distances
	 * at a given point.
	 */
	inline void GetDistances_(const Point &pos, double &longitudinal, double &latitudinal)
	{
		Point delta = pos - center_;
		longitudinal = delta.AngleRad()*radius_; // equivalent to rad*c/2pi
		latitudinal = fabs(delta.Norm()-radius_);
	}

	inline double GetLatitudinalDistance_(const Point &pos)
	{
		Point delta = pos - center_;
		return fabs(delta.Norm()-radius_);
	}


	virtual std::string GetName();

	StrokePtr GetStroke() const	{ LATERO_GRAPHICS_GUARD; return stroke_; }

	void SetCenter(const Point &v);
	Point GetCenter() const		{ LATERO_GRAPHICS_GUARD; return center_; }

	void SetRadius(float v);
	float GetRadius() const		{ LATERO_GRAPHICS_GUARD; return radius_; }

	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);
	virtual bool UpdateVisible_(const State *state);

protected:
	Circle(const latero::Tactograph *dev, Point center, double radius);
	Circle(const latero::Tactograph *dev, const XMLInputNode &node);

	inline double GetCircumference() { LATERO_GRAPHICS_GUARD; return GetCircumference_(); };
	inline double GetCircumference_() { return 2*M_PI*radius_; };

	Point center_;
	double radius_;
	StrokePtr stroke_;

	DottedCirclePtr dots_;	// dots rendering
};

} // namespace

