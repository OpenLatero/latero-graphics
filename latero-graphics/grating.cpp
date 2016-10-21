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

#include "grating.h"
#include "gratingmodulator.h"
#include <iostream>

/**
 * Interp:
 *
 * This parameter controls the interpolation of the sinusoid when the 'center'
 * is not in the middle of the range. This can be understood as a remapping of 
 * the position such that the points (0,0), (center, 0.5) and (1,1) are crossed.
 * The basic INTERP_LINEAR interpolation creates this mapping with straight lines
 * and results in a discontinous slope in the sinusoid at the 'center'. The INTERP_ARC
 * interpolation fits an arc through the 3 points and results in a much smoother
 * sinusoid.
 * 
 * The algorithm used can be easilly derived starting from the equation of a circle with 
 * unknow center and radius. The equations used here are obtained by plugging in the three
 * points above. Note that there is a jump in the position of the circle's center when
 * the center crosses the middle position, hence the sign change (pick upper or lower y).
 *
 * TODO: Determine if this interpolation makes a difference in the sensation. If so,
 * look for better, more efficient methods. The transition with the gaps could similarly
 * be smoothed, possibly in one step with multiple "knots", e.g. with B-splines,
 */


namespace latero {
namespace graphics { 

Grating::Grating(std::string units) :
	ridgeSize_(1),
	gapSize_(0),
	center_(center_default),
	units_(units),
	velUnitsHz_(true),
	vel_(0),
	enable_(true),    
	length_(1),
	invert_(invert_default),    
	amplitude_(1.0),
    interpMode_(interp_default),
    regularizable_(false)
{
	mod_ = GratingModulator::Create(units);
	AddModifiableChild(mod_);
}

double Grating::Render_(double pos, boost::posix_time::time_duration timeElapsed)
{
	if (!enable_) return (invert_?1:0);
	double cycleSize = GetCycleSize_();

	if (mod_->GetEnable_())
	{
		pos = mod_->ModulatePosition_(pos);
	}
	else
	{
		// apply velocity effect
		double distVel = GetVelDist_();
		if (fabs(distVel)>0)
		{
			float us = timeElapsed.total_microseconds() % (long)(GetRhythmSize_()*1E6/fabs(distVel)); // reduce to a manageable number
			pos -= distVel*us/1E6; // the texture moves against the position
		}
	}

	// normalize the position 
	float p = pos/cycleSize;

	// find the position between 0 and 1 within this cycle
	float posInCycle = fmod(p,1); 
	if (posInCycle<0) posInCycle+=1;

	// find the number of this cycle
	int cycle = floor(p);

	double rv = RenderCycle_(posInCycle);
	if (rhythm_.size()) rv *= GetRhythmEffect_(cycle);

	rv *=amplitude_;
	if (invert_) rv = 1-rv; 
	return rv;
}

double Grating::RenderCycle_(double pos)
{
	if (pos>(1-GetGap_()))
	{
		return 0.0;
	}
	else
	{
		/**
		 * remap the position such that the waveform occupies the
		 * range from 0 to 1, even if there is a gap.
		 */
		pos = pos/(1-GetGap_());

		// offset the center if necessary
		if (center_ != 0.5)
		{
			if (interpMode_ == interp_linear)
			{
				if (pos<center_)
					pos = (pos/center_)*0.5;
				else
					pos = 0.5+((pos-center_)/(1-center_))*0.5;
			}
			else // if (interpMode_ == interp_arc)
			{
				double dx = pos-arcX0_;
				double t = sqrt(arcR2_ - dx*dx);
				double y = (center_<0.5)?arcY0_+t:arcY0_-t;
				pos = fmin(fmax(y,0),1); // allow for some error...
			}
		}

		assert((pos>=0) && (pos<=1));
		return 0.5*(1-cos(pos*2*M_PI));
	}
}


double Grating::GetRhythmEffect_(int cycle)
{
	double rv = 1.0;
	int n = rhythm_.size();
	if (n)
	{
		cycle = cycle % n;
		if (cycle<0) cycle+=n;
		rv = rhythm_[cycle];
	}
	return rv;
}


void Grating::SetCenter(double v)
{
	LATERO_GRAPHICS_GUARD;
	center_ = ClipCenter(v);
	SetLastModified_();

	// precompute parameters for ARC approximation
	double c = center_;
	arcX0_ = (3-4*c*c)/(4-8*c);
	arcY0_ = 1-arcX0_;
	arcR2_ = arcX0_*arcX0_ + arcY0_*arcY0_;
}

void Grating::Regularize_(double length)
{
	int n = round(length/GetCycleSize_());
	float size = length/n;

	if (gapSize_ == 0)
	{
		ridgeSize_ = size;
	}
	else
	{
		// try to maintain ridge size
		if (size > ridgeSize_)
			gapSize_ = size - ridgeSize_;
		else
			SetCycleSize_(size);
	}
	SetLastModified_();
	std::cout << "Regularize: ridge=" << ridgeSize_ << ", gap=" << gapSize_ << "\n";
}

void Grating::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Grating");

	node.AddChild("enable", GetEnable());
	node.AddChild("ridge_size", GetRidgeSize(), GetUnits());
	node.AddChild("gap_size", GetGapSize(), GetUnits());
	if (GetVel() != 0) node.AddChild("velocity", GetVel(), GetVelUnits());
	if (GetInvert()) node.AddChild("invert", GetInvert());
	mod_->AppendXML(node);
}

void Grating::LoadXML(const XMLInputNode &root)
{
	XMLInputNode gnode = root.GetChild("Grating");

	XMLInputNode node = gnode.GetChild("enable");
	if (node) SetEnable(node.GetBool());

	node = gnode.GetChild("ridge_size");
	if (node) SetRidgeSize(node.GetFloat());

	node = gnode.GetChild("gap_size");
	if (node) SetGapSize(node.GetFloat());

	node = gnode.GetChild("velocity");
	if (node) 
	{
		SetVelUnitsHz(node.GetUnits() == units::hz);
		SetVel(node.GetFloat());
	}

	node = gnode.GetChild("invert");
	if (node) SetInvert(node.GetBool());

	mod_->LoadXML(gnode);
}

void Grating::SetVelUnitsHz(bool v)
{
	LATERO_GRAPHICS_GUARD; 
	if (velUnitsHz_!=v)
	{
		velUnitsHz_=v; 
		if (velUnitsHz_)
			vel_ /= GetCycleSize_();
		else
			vel_ *= GetCycleSize_();
		SetLastModified_();
	}
}

void Grating::SetUnits(std::string v)
	 { LATERO_GRAPHICS_GUARD; units_=v; mod_->SetUnits(v); }

double Grating::GetMinRidgeSize()
{
	double rv = GetRidgeSize();
	if (GetModulator()->GetEnable())
		rv = fmin(rv, rv/GetModulator()->GetFactor());
	return rv;
}

double Grating::GetMinCycleSize_()
{
	double rv = GetCycleSize_();
	if (GetModulator()->GetEnable_())
		rv = fmin(rv, rv/GetModulator()->GetFactor_());
	return rv;
}


} // namespace graphics
} // namespace latero
 
