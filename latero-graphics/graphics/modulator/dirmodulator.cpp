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

#include "dirmodulator.h"
#include "dirmodulatorwidget.h"
#include <gtkmm/box.h>
#include "../../units.h"

namespace latero {
namespace graphics { 

static const std::string low_vel_mode_high_str = "high";
static const std::string low_vel_mode_low_str = "low";
static const std::string low_vel_mode_toggle_str = "toggle";

DirModulator::DirModulator(const latero::Tactograph *dev) :
	Modulator(dev),
	dir_(dir_default),
	dirTolerance_(dirTolerance_default),
	dirTransition_(dirTransition_default),
	velTolerance_(velTolerance_default),
	velTransition_(velTransition_default),
	symmetric_(symmetric_default),
	lowVelMode_(lowVelMode_default),
	toggleLowVelHigh_(true)
{
}

DirModulator::DirModulator(const latero::Tactograph *dev, const XMLInputNode &node) :
	Modulator(dev),
	dir_(dir_default),
	dirTolerance_(dirTolerance_default),
	dirTransition_(dirTransition_default),
	velTolerance_(velTolerance_default),
	velTransition_(velTransition_default),
	symmetric_(symmetric_default),
	lowVelMode_(lowVelMode_default),
	toggleLowVelHigh_(true)
{
	LoadXML(node);
}


Gtk::Widget *DirModulator::CreateWidget(ModulatorPtr ptr)
{
	DirModulatorPtr gen = boost::dynamic_pointer_cast<DirModulator>(ptr);
	if (!gen) return Modulator::CreateWidget(ptr); // default
	return new DirModulatorWidget(gen);
}

double DirModulator::GetValue_(const ActuatorState &state)
{
	// TODO: this should be computed once, not for each actuator

	double deviation = units::RadToDegree(state.velDir) - dir_;
	deviation = units::DegreeShift(deviation, -180);
	deviation = fabs(deviation);
	if (symmetric_)
		if (deviation>90) 
			deviation = 180-deviation;

	// handle velocity (and reversals)
	double magEffect = 1.0 - (state.velMag - velTolerance_) / velTransition_;
	magEffect = fmin(fmax(magEffect,0.0),1.0);
	
	// handle deviation
	double devEffect = 1.0 - (deviation - dirTolerance_) / dirTransition_;
	devEffect = fmin(fmax(devEffect,0.0),1.0);

	double rv = 1;
	if ((lowVelMode_==low_vel_mode_high) || ((lowVelMode_ == low_vel_mode_toggle)&&toggleLowVelHigh_))
		rv = fmax(magEffect,devEffect);
	else if ((lowVelMode_ == low_vel_mode_low) || ((lowVelMode_ == low_vel_mode_toggle)&&!toggleLowVelHigh_))
		rv = fmin(devEffect,(1-magEffect));
		//rv = devEffect * (1-magEffect);

	// Don't update the internal state when simulating.(?!)
	if (state.mode.StateChangeAllowed()) 
	{
		if (toggleLowVelHigh_ && rv==0)
			toggleLowVelHigh_ = false;
		else if (!toggleLowVelHigh_ && rv==1)
			toggleLowVelHigh_ = true;
	}

	return rv;
}

void DirModulator::SetDirTolerance(double v)
{
	LATERO_GRAPHICS_GUARD;
	dirTolerance_= v;
	SetLastModified_();
}

void DirModulator::SetDirTransition(double v)
{
	LATERO_GRAPHICS_GUARD;
	dirTransition_= fmin(v,180-dirTolerance_);
	SetLastModified_();
}

void DirModulator::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Modulator");
	node.SetAttribute("type",GetXMLType());
	node.AddChild("direction", GetDirection(), units::degree);
	node.AddChild("direction_tolerance", GetDirTolerance(), units::degree);
	node.AddChild("direction_transition", GetDirTransition(), units::degree);
	node.AddChild("velocity_tolerance", GetVelTolerance(), units::mm_per_sec);
	node.AddChild("velocity_transition", GetVelTransition(), units::mm_per_sec);
	node.AddChild("symmetric", GetSymmetric());
	node.AddChild("low_velocity_mode", GetLowVelModeLabel());
}

void DirModulator::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("direction");
	if (node) SetDirection(node.GetFloat());

	node = root.GetChild("direction_tolerance");
	if (node) SetDirTolerance(node.GetFloat());

	node = root.GetChild("direction_transition");
	if (node) SetDirTransition(node.GetFloat());

	node = root.GetChild("velocity_tolerance");
	if (node) SetVelTolerance(node.GetFloat());

	node = root.GetChild("velocity_transition");
	if (node) SetVelTransition(node.GetFloat());

	node = root.GetChild("symmetric");
	if (node) SetSymmetric(node.GetBool());

	node = root.GetChild("low_velocity_mode");
	if (node) SetLowVelModeLabel(node.GetString());

}

void DirModulator::SetLowVelModeLabel(std::string label)
{
	if (label == low_vel_mode_high_str)
		SetLowVelMode(low_vel_mode_high);
	else if (label == low_vel_mode_low_str)
		SetLowVelMode(low_vel_mode_low);
	else if (label == low_vel_mode_toggle_str)
		SetLowVelMode(low_vel_mode_toggle);
}


std::string DirModulator::GetLowVelModeLabel() const
{
	int mode = GetLowVelMode();
    
	if (mode == low_vel_mode_high)
		return low_vel_mode_high_str;
	else if (mode == low_vel_mode_low)
		return low_vel_mode_low_str;
	else if (mode == low_vel_mode_toggle)
		return low_vel_mode_toggle_str;
    else
        return low_vel_mode_high_str; // todo: ok?
}

std::vector<std::string> DirModulator::GetLowVelModeLabels()
{
	std::vector<std::string> rv;
	rv.push_back(low_vel_mode_high_str);
	rv.push_back(low_vel_mode_low_str);
	rv.push_back(low_vel_mode_toggle_str);
	return rv;
}

} // namespace graphics
} // namespace latero



