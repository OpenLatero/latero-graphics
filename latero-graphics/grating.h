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

#ifndef LATERO_GRAPHICS_GRATING_H
#define LATERO_GRAPHICS_GRATING_H

#include "gratingfwd.h"
#include "gratingmodulatorfwd.h"
#include "modifiable.h"
#include "xml.h"
#include "units.h"

namespace latero {
namespace graphics { 

class Grating : public Modifiable
{
public:
	typedef enum { interp_linear=0, interp_arc=1 } InterpMode;

	static const bool invert_default = false;
    constexpr static const double center_default = 0.5;
    constexpr static const double center_min = 0.15;
    constexpr static const double center_max = 0.85;
    constexpr static const double gap_default = 0.0;
    constexpr static const double gap_min = 0.0;
    constexpr static const double gap_max = 0.9;
	static const InterpMode interp_default = interp_linear;

	static GratingPtr Create(std::string units="")
	{
		return GratingPtr(new Grating(units));
	}

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	// position in "units"
	double Render_(double pos, boost::posix_time::time_duration timeElapsed);

	inline double GetCenter() const  { LATERO_GRAPHICS_GUARD; return center_; }
	void SetCenter(double v);

	inline double GetGap_() const { return gapSize_/GetCycleSize_(); }
	inline double GetGap() const  { LATERO_GRAPHICS_GUARD; return GetGap_(); }
	inline void SetGap(double v)
	{
		LATERO_GRAPHICS_GUARD;
		double size = GetCycleSize_();
		double percent = ClipGap(v);
		gapSize_= percent*size;
		ridgeSize_= size-gapSize_;
		SetLastModified_();
	}

	inline InterpMode GetInterp() const  { LATERO_GRAPHICS_GUARD; return interpMode_; }
	inline void SetInterp(InterpMode v) { LATERO_GRAPHICS_GUARD; interpMode_=ClipInterp(v); SetLastModified_(); }

	inline std::string GetUnits() const  { LATERO_GRAPHICS_GUARD; return units_; }
	void SetUnits(std::string v);

	inline double GetCycleSize_() const  { return gapSize_+ridgeSize_; }
	inline double GetCycleSize() const  { LATERO_GRAPHICS_GUARD; return GetCycleSize_(); }
	inline void SetCycleSize(double v) { LATERO_GRAPHICS_GUARD; SetCycleSize_(v); }
	inline void SetCycleSize_(double v) 
	{ 
		double gap = GetGap_();
		gapSize_= gap*v;
		ridgeSize_= v-gapSize_;
		SetLastModified_(); 
	}

	inline double GetRhythmSize_() const 
	{
		double rv = GetCycleSize_();
		if (rhythm_.size()) rv *= rhythm_.size();
		return rv;
	}
	inline double GetRhythmSize() const  { LATERO_GRAPHICS_GUARD; return GetRhythmSize_(); }

	inline bool GetEnable()  const { LATERO_GRAPHICS_GUARD; return GetEnable_(); }
	inline bool GetEnable_()  const { return enable_; }
	inline void SetEnable(bool v) { LATERO_GRAPHICS_GUARD; enable_=v; SetLastModified_(); }

	inline double GetVel()  const { LATERO_GRAPHICS_GUARD; return vel_; }
	inline void SetVel(double v) { LATERO_GRAPHICS_GUARD; vel_=v; SetLastModified_(); }

	inline std::string GetVelUnits() const { LATERO_GRAPHICS_GUARD; return (velUnitsHz_?units::hz:GetUnits()+units::per_sec); }
	inline bool GetVelUnitsHz()  const { LATERO_GRAPHICS_GUARD; return velUnitsHz_; }
	void SetVelUnitsHz(bool v);

	inline double GetRidgeSize() const { LATERO_GRAPHICS_GUARD; return ridgeSize_; }
	inline void SetRidgeSize(double v) { LATERO_GRAPHICS_GUARD; ridgeSize_=v; SetLastModified_(); }

	inline double GetGapSize() const { LATERO_GRAPHICS_GUARD; return gapSize_; }
	inline void SetGapSize(double v) { LATERO_GRAPHICS_GUARD; gapSize_=v; SetLastModified_(); }

	inline std::vector<double> GetRhythm() const { LATERO_GRAPHICS_GUARD; return rhythm_; }
	inline void SetRhythm(const std::vector<double> &v) { LATERO_GRAPHICS_GUARD; rhythm_=v; SetLastModified_(); }
	
	inline bool GetInvert() const { LATERO_GRAPHICS_GUARD; return invert_; }
	inline void SetInvert(bool v) { LATERO_GRAPHICS_GUARD; invert_=v; SetLastModified_(); }

	inline double GetAmplitude() const { LATERO_GRAPHICS_GUARD; return amplitude_; }
	inline void SetAmplitude(double v) { LATERO_GRAPHICS_GUARD; amplitude_=v; SetLastModified_(); }

	/**
	 * Modifies the grating parameters such that an integer number of cycles
	 * fits within the specified 'length'. This is useful when a grating must wrap 
	 * around (e.g. on a circle) or must end cleanly at a specific position.
	 */
	inline void Regularize() { LATERO_GRAPHICS_GUARD; Regularize_(length_); }
	inline void Regularize(double length) { LATERO_GRAPHICS_GUARD; Regularize_(length); }
	void Regularize_(double length);
	inline void SetLength(double v) { LATERO_GRAPHICS_GUARD; length_=v; /*SetLastModified_();*/ }
	inline double GetLength_() { return length_; }

	// controls pitch modulation
	GratingModulatorPtr GetModulator() { return mod_; }

	// hack
	double GetMinRidgeSize();
	double GetMinCycleSize_();

	inline bool GetRegularizable() const { return regularizable_; }
	inline void SetRegularizable(bool v) { regularizable_=v; }
protected:

	// @return velocity in units per second
	inline double GetVelDist_()  const { return (velUnitsHz_ ? vel_*GetCycleSize_() : vel_); }

	/**
	 * @return the deflection at a given position in a cycle of the grating
	 * @param pos position between 0 and 1
	 */
	double RenderCycle_(double pos);

	double GetRhythmEffect_(int cycle);

	Grating(std::string unit);

	inline double ClipCenter(double v) const
		{ return fmin(fmax(v,center_min),center_max); }
	inline double ClipGap(double v) const
		{ return fmin(fmax(v,gap_min),gap_max); }
	inline InterpMode ClipInterp(InterpMode v) const
		{ return ((v==interp_linear)||(v==interp_arc))?v:interp_default; }

	double ridgeSize_; // size of bump in units
	double gapSize_; 	// size of gap in units

	double center_; // location of center (between 0 to 1)
	std::string units_; // unit of distance (for displaying to user)

	/**
	 * The velocity can either be specified in Hz (cycles per second) or in distance units 
	 * (usually mm or degree). 
	 */
	bool velUnitsHz_;	// vel_ is in Hz if true, "units" per second otherwise
	double vel_; 		// velocity in "units" per second or Hz

	bool enable_;
	double length_; 	// used for regularization

	bool invert_;		// invert from 0-1 to 1-0
	double amplitude_;	// scaling factor

	std::vector<double> rhythm_;

	InterpMode interpMode_;
	double arcX0_,arcY0_,arcR2_;

	GratingModulatorPtr mod_;

	bool regularizable_;
};

} // namespace graphics
} // namespace latero

#endif
