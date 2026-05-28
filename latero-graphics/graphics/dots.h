#pragma once

#include "dotsfwd.h"
#include "group.h"
#include "../ridgefwd.h"
#include "../oscillator.h"
#include "dot.h"

namespace latero::graphics {

/** Set of dots with all the same properties (except location). */
class Dots : public Group
{
public:

	static DotsPtr Create(const latero::Tactograph *dev, const XMLInputNode &root)
	{
		return DotsPtr(new Dots(dev, root));
	}


	static DotsPtr Create(const latero::Tactograph *dev, double radius=1.0, double value=1.0)
	{
		return DotsPtr(new Dots(dev, radius, value));
	}

	static DotsPtr Create(const latero::Tactograph *dev, const std::vector<Point> &points, double radius=1.0, double value=1.0)
	{
		return DotsPtr(new Dots(dev, points, radius, value));
	}
	
	virtual ~Dots() {};

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	virtual void AppendXML(XMLOutputNode &root) const;
	void AppendHeight(XMLOutputNode &node) const;
	void AppendDotRadius(XMLOutputNode &node) const;
	void AppendVib(XMLOutputNode &node) const;
	void AppendProfile(XMLOutputNode &node) const;

	virtual void LoadXML(const XMLInputNode &root);
	void LoadPoints(const XMLInputNode &root);
	void LoadProfile(const XMLInputNode &root);
	void LoadVib(const XMLInputNode &root);
	void LoadHeight(const XMLInputNode &root);
	void LoadDotRadius(const XMLInputNode &root);

	void InsertPoints(const std::vector<Point> &points);
	void SetPoints(const std::vector<Point> &points);
	void InsertPoint(const Point &p, bool invertVib=false);
	std::vector<Point> GetPoints() const;
	void SetPoint(int i, const Point &p);
	Point GetPoint(int i) const;
	void ClearPoints() { LATERO_GRAPHICS_GUARD; ClearPoints_(); }
	void ClearPoints_() { Group::ClearPatterns_(); }

	virtual std::string GetName() { return "Dots"; }
	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

	void SetProfile(RidgePtr v);
	RidgePtr GetProfile() { LATERO_GRAPHICS_GUARD; return profile_; }
	RidgePtr GetProfile_() { return profile_; }

	void SetOscillator(OscillatorPtr vib);
	OscillatorPtr GetOscillator() { LATERO_GRAPHICS_GUARD; return vib_; }

	inline double GetDotRadius() const	{ LATERO_GRAPHICS_GUARD; return GetDotRadius_(); }
	inline double GetDotRadius_() const	{ return dotRadius_; }
	void SetDotRadius(float v);

	float GetHeight() const;
	void SetHeight(float v);

	virtual bool ChildrenArePublic() { return false; }

protected:
	// not for realtime use!
	std::vector<DotPtr> GetDots() const;

	//std::vector<DotPtr> dotSet_; 	// positions within virtual surface
	float height_;			// from 0 to 1
	float dotRadius_;		// radius in mm (conflicts with DottedCircle's radius
	RidgePtr profile_;		// shape of the ridge
	OscillatorPtr vib_;		// vibration

	Dots(const latero::Tactograph *dev, const std::vector<Point> &points, double radius, double value);
	Dots(const latero::Tactograph *dev, double radius=1, double value=1);
	Dots(const latero::Tactograph *dev, const XMLInputNode &root);
};

} // namespace

