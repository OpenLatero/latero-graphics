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

#ifndef LATERO_GRAPHICS_PLANAR_DIR_MODULATOR_H
#define LATERO_GRAPHICS_PLANAR_DIR_MODULATOR_H

#include "dirmodulatorfwd.h"
#include "modulator.h"
#include "../../units.h"
#include <string>

namespace latero {
namespace graphics { 

/**
 * Modulates based on the direction of the exloration velocity (and to some extend the magnitude).
 */
class DirModulator : public Modulator
{
public:
	enum { low_vel_mode_high, low_vel_mode_low, low_vel_mode_toggle };

	static const double dir_default = 0;
	static const double dirTolerance_default = 30;
	static const double dirTransition_default = 20;
	static const double velTolerance_default = 0;
	static const double velTransition_default = 5;
	static const bool symmetric_default = false;
	static const int lowVelMode_default = low_vel_mode_toggle;

	static DirModulatorPtr Create(const latero::Tactograph *dev)
		{ return DirModulatorPtr(new DirModulator(dev)); }

	static DirModulatorPtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
		{ return DirModulatorPtr(new DirModulator(dev,node)); }
	
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual ~DirModulator() {}
	virtual Gtk::Widget *CreateWidget(ModulatorPtr ptr);
	virtual double GetValue_(const ActuatorState &state);

	void SetDirection(double v) { LATERO_GRAPHICS_GUARD; dir_= units::DegreeShift(v,0); SetLastModified_(); }
	double GetDirection() const { LATERO_GRAPHICS_GUARD; return dir_; }

	void SetDirTolerance(double v);
	double GetDirTolerance() const { LATERO_GRAPHICS_GUARD; return dirTolerance_; }

	void SetDirTransition(double v);
	double GetDirTransition() const { LATERO_GRAPHICS_GUARD; return dirTransition_; }

	void SetVelTolerance(double v) { LATERO_GRAPHICS_GUARD; velTolerance_ = v; SetLastModified_(); }
	double GetVelTolerance() const { LATERO_GRAPHICS_GUARD; return velTolerance_; }

	void SetVelTransition(double v) { LATERO_GRAPHICS_GUARD; velTransition_ = v; SetLastModified_(); }
	double GetVelTransition() const { LATERO_GRAPHICS_GUARD; return velTransition_; }

	void SetSymmetric(bool v) { LATERO_GRAPHICS_GUARD; symmetric_ = v; SetLastModified_(); }
	bool GetSymmetric() const { LATERO_GRAPHICS_GUARD; return symmetric_; }

	void SetLowVelModeLabel(std::string label);
	std::string GetLowVelModeLabel() const;
	static std::vector<std::string> GetLowVelModeLabels();

	void SetLowVelMode(int v) { LATERO_GRAPHICS_GUARD; lowVelMode_ = v; SetLastModified_(); }
	int GetLowVelMode() const { LATERO_GRAPHICS_GUARD; return lowVelMode_; }

	virtual void LoadXML(const XMLInputNode &node);
	static std::string GetXMLType() { return "Directional"; }
protected:
	DirModulator(const latero::Tactograph *dev);
	DirModulator(const latero::Tactograph *dev, const XMLInputNode &node);

	double dir_; // preferred direction (degree [0,360[)
	double dirTolerance_; // modulator is high for dir ± tolerance
	double dirTransition_; // size of the transition from high to low
	double velTolerance_;
	double velTransition_;
	bool symmetric_; // if true, modulator is high in both directions along the direction axis
	int lowVelMode_; // determines if the low-velocity region should be low or high
	bool toggleLowVelHigh_;
};

} // namespace graphics
} // namespace latero

#endif

