#pragma once

#include "velmodulatorfwd.h"
#include "modulator.h"
#include "../../units.h"

namespace latero::graphics {

class VelModulator : public Modulator
{
public:
    constexpr static const double limit_default = 5;
    constexpr static const double transition_default = 5;
    constexpr static const double delay_default = 500;

	static VelModulatorPtr Create(const latero::Tactograph *dev)
		{ return VelModulatorPtr(new VelModulator(dev)); }

	static VelModulatorPtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
		{ return VelModulatorPtr(new VelModulator(dev,node)); }
	
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual ~VelModulator() {}
	virtual Gtk::Widget *CreateWidget(ModulatorPtr ptr);
	virtual double GetValue_(const ActuatorState &state);
	
    inline double GetLastValue() const { LATERO_GRAPHICS_GUARD; return GetLastValue_(); }
    inline double GetLastValue_() const { return lastValue_; }
    
	void SetTransition(double v) { LATERO_GRAPHICS_GUARD; transition_ = v; SetLastModified_(); }
	double GetTransition() const { LATERO_GRAPHICS_GUARD; return transition_; }

	void SetLimit(double v) { LATERO_GRAPHICS_GUARD; limit_ = v; SetLastModified_(); }
	double GetLimit() const { LATERO_GRAPHICS_GUARD; return limit_; }

	void SetDelay(double v) { LATERO_GRAPHICS_GUARD; delay_ = v; SetLastModified_(); }
	double GetDelay() const { LATERO_GRAPHICS_GUARD; return delay_; }

	virtual void LoadXML(const XMLInputNode &node);
	static std::string GetXMLType() { return "Velocity"; }
    
    // todo: not sure this is a reasonable measure (scrap?)
    inline long GetToggleCount_() const { return toggleCount_; }
    inline long GetToggleCount() const { LATERO_GRAPHICS_GUARD; return GetToggleCount_(); }
    
protected:
	VelModulator(const latero::Tactograph *dev);
	VelModulator(const latero::Tactograph *dev, const XMLInputNode &node);

	double limit_; 		// velocity range for which modulation is high 
	double transition_;	// size of the transition from high to low
	double delay_;		// delay in ms before transition to high kicks in

	// internal state
	bool stateOut_;
	boost::posix_time::time_duration startTime_;
    double lastValue_;
    long toggleCount_;
};

} // namespace

