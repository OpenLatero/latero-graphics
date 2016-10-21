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

#ifndef LATERO_GRAPHICS_PLANAR_VEL_MODULATOR_H
#define LATERO_GRAPHICS_PLANAR_VEL_MODULATOR_H

#include "velmodulatorfwd.h"
#include "modulator.h"
#include "../../units.h"

namespace latero {
namespace graphics { 

class VelModulator : public Modulator
{
public:
	static const double limit_default = 5;
	static const double transition_default = 5;
	static const double delay_default = 500;

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

} // namespace graphics
} // namespace latero

#endif

