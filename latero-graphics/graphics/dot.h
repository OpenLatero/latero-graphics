#pragma once

#include <memory>
#include "pattern.h"
#include "../ridge.h"
#include "../oscillator.h"

namespace latero::graphics {

typedef std::shared_ptr<class Dot> DotPtr;

class Dot : public Pattern
{
public:

	static DotPtr Create(const latero::Tactograph *dev, const XMLInputNode &root)
	{
		return DotPtr(new Dot(dev, root));
	}


	static DotPtr Create(const latero::Tactograph *dev, const Point &pos=Point(0,0), double radius=1.0, double value=1.0, bool invertVib=false)
	{
		return DotPtr(new Dot(dev, pos, radius, value, invertVib));
	}
	
	virtual ~Dot() {};

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

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);
	virtual std::string GetName() { return "Dot"; }
	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

	void SetProfile(RidgePtr v);
	RidgePtr GetProfile() { LATERO_GRAPHICS_GUARD; return profile_; }
	RidgePtr GetProfile_() { return profile_; }

	OscillatorPtr GetOscillator() 		{ LATERO_GRAPHICS_GUARD; return vib_; }
	void SetOscillator(OscillatorPtr p);

	inline double GetDotRadius() const	{ LATERO_GRAPHICS_GUARD; return GetDotRadius_(); }
	inline double GetDotRadius_() const	{ return dotRadius_; }
	void SetDotRadius(float v);

	float GetHeight() const	 { LATERO_GRAPHICS_GUARD; return height_; }
	void SetHeight(float v)  { LATERO_GRAPHICS_GUARD; height_=v; SetLastModified_();}

	Point GetPos() const { LATERO_GRAPHICS_GUARD; return pos_; }
	void SetPos(Point p) { LATERO_GRAPHICS_GUARD; pos_=p; SetLastModified_(); }

	void SetInvertVib(bool v) { LATERO_GRAPHICS_GUARD; invertVib_ = v; SetLastModified_(); }
	bool GetInvertVib() const { LATERO_GRAPHICS_GUARD; return invertVib_; }

	virtual bool UpdateVisible_(const State *state);

protected:
	Point pos_;
	bool invertVib_;
	float height_;			// from 0 to 1
	float dotRadius_;		// radius in mm (conflicts with DottedCircle's radius
	RidgePtr profile_;		// shape of the ridge
	OscillatorPtr vib_;		// vibration

	Dot(const latero::Tactograph *dev, const Point &pos, double radius, double value, bool invertVib);
	Dot(const latero::Tactograph *dev, const XMLInputNode &root);
};


} // namespace

