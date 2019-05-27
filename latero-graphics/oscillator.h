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

#ifndef LATERO_GRAPHICS_OSCILLATOR_H
#define LATERO_GRAPHICS_OSCILLATOR_H

#include <cairomm/context.h>
#include "oscillatorfwd.h"
#include "modifiable.h"
#include "xml.h"
#include "state.h"
#include "namedid.h"

namespace latero {
namespace graphics { 

/** 
 * The oscillator efficiently handles the rendering of vibrations.
 */
class Oscillator  : public Modifiable
{
public:
	typedef enum { mode_normal=0, mode_force_enable=1, mode_invert_enable=2 } enable_mode;

	typedef NamedID BlendMode;
	typedef NamedIDSet BlendModeSet;
	static const BlendMode blend_mode_high, blend_mode_low;

    constexpr static const double freq_default = 50.0;
    constexpr static const double freq_min = 0.1;
    constexpr static const double freq_max = 50.0;
    constexpr static const double amplitude_default = 1.0;

	static OscillatorPtr Create(double freq, double amplitude, bool enable)
	{
		return OscillatorPtr(new Oscillator(freq,amplitude,enable));
	}

	virtual ~Oscillator() {}

	void LoadXML(const XMLInputNode &root);
	void AppendXML(XMLOutputNode &root) const;

	// optimized to compute twice per refresh, instead of 64 times
	inline double Render_(const ActuatorState &state, enable_mode enMode = mode_normal)
	{
		bool en = (enMode == mode_force_enable) ? true : enable_;
		if (enMode == mode_invert_enable) en = !en;
		if (!en) return 0;
		if (state.mode.VibAsNoise()) return amplitude_ *(rand()%1000)/1000;

		if ((state.GetTimeElapsed()!=lastTime_)||(mod_))
		{
			lastTime_=state.GetTimeElapsed();
			mod_=false;

			double t = state.GetTimeElapsed().total_microseconds() % period_.total_microseconds(); // take the modulus of the time to keep good resolution
			values[0] = 0.5 + 0.5*sin(2.0*M_PI*t/period_.total_microseconds());
			values[1] = 1-values[0];
		}

		return amplitude_ * (((state.i%2) == (state.j%2)) ? values[0] : values[1]);

		/*
		double t = state.time % period_; // take the modulus of the time to keep good resolution
		values[0] = 0.5 + 0.5*sin(2.0*M_PI*t/period_);
		values[1] = 1-values[0];
		return amplitude_ * (((state.i%2) == (state.j%2)) ? values[0] : values[1]);
		*/
	}

	inline double RenderBlend_(const ActuatorState &state, enable_mode enMode = mode_normal)
	{
		bool en = (enMode == mode_force_enable) ? true : enable_;
		if (enMode == mode_invert_enable) en = !en;
		if (!en) return 1;
		return ((1-amplitude_) + Render_(state,enMode));
	}

	inline double RenderBlend_(double deflection, const ActuatorState &state, bool invertVib=false)
	{
		if (!enable_ && !invertVib) return deflection;

		bool blendHigh = (blendMode_ == blend_mode_high);
		if (invertVib && enable_) blendHigh = !blendHigh;

		double vib = RenderBlend_(state,mode_force_enable);
		if (blendHigh)
			return deflection * vib;
		else
			return 1-vib*(1-deflection);
	}

	// Frequency

	/** @return frequency in Hz (locking) */
	inline double GetFreq() const { LATERO_GRAPHICS_GUARD; return GetFreq_(); }

	/** @return frequency in Hz (non-locking) */
	inline double GetFreq_() const { return 1E6/period_.total_microseconds(); }

	/** set frequency in Hz (locking) */
	inline void SetFreq(double v) { LATERO_GRAPHICS_GUARD; SetFreq_(v); }

	/** set frequency in Hz (non-locking) */
	inline void SetFreq_(double v) { SetLastModified_(); v = fmin(freq_max,fmax(freq_min,v)); period_ = boost::posix_time::microseconds(1E6/v); mod_=true; }

	// Amplitude

	/** @return amplitude from 0 to 1 (locking) */
	inline double GetAmplitude() const { LATERO_GRAPHICS_GUARD; return GetAmplitude_(); }

	/** @return amplitude from 0 to 1 (non-locking) */
	inline double GetAmplitude_() const { return amplitude_; }

	/** set amplitude from 0 to 1 (locking) */
	inline void SetAmplitude(double v)  { LATERO_GRAPHICS_GUARD; SetAmplitude_(v); }

	/** set amplitude from 0 to 1 (non-locking) */
	inline void SetAmplitude_(double v)  { amplitude_=fmax(0,fmin(1,v)); SetLastModified_(); }

	/**
	 * Enable.
	 */
	inline bool GetEnable() const { LATERO_GRAPHICS_GUARD; return GetEnable_(); }
	inline bool GetEnable_() const { return enable_; }
	inline void SetEnable(bool v)  { LATERO_GRAPHICS_GUARD; SetEnable_(v); }
	inline void SetEnable_(bool v)  { enable_=v; SetLastModified_(); }

	/**
	 * Blend mode.
	 */
	BlendModeSet GetBlendModes() const { return blendModes_; };
	BlendMode GetBlendMode() const { LATERO_GRAPHICS_GUARD; return blendMode_; }
	void SetBlendMode(const BlendMode &mode) { LATERO_GRAPHICS_GUARD; blendMode_ = mode; SetLastModified_(); }
	void SetBlendMode(std::string label);

	// when used with Render_
	Cairo::RefPtr<Cairo::Pattern> GetPattern(Cairo::RefPtr<Cairo::Context> mmContext) {
		return GetPattern(mmContext,1); 
	}

	// when used with RenderBlend_
	Cairo::RefPtr<Cairo::Pattern> GetBlendPattern(Cairo::RefPtr<Cairo::Context> mmContext) {
		return GetPattern(mmContext,GetAmplitude()); 
	}

	// when used with RenderBlend_(deflection)
	Cairo::RefPtr<Cairo::Pattern> GetBlendPattern(Cairo::RefPtr<Cairo::Pattern> base, Cairo::RefPtr<Cairo::Context> mmContext);


protected:
	Cairo::RefPtr<Cairo::Pattern> GetPattern(Cairo::RefPtr<Cairo::Context> mmContext, double k);
	Oscillator(double freq, double amplitude, bool enable);

	double values[2];
	bool mod_;
	boost::posix_time::time_duration lastTime_;

	boost::posix_time::time_duration period_;
	double amplitude_;	// amplitude of the vibration between 0 and 1
	bool enable_;		// enable or disables vibration

	BlendMode blendMode_;	// determines whether high or low activation is vibrated (in blend mode only)
	BlendModeSet blendModes_;
};

} // namespace graphics
} // namespace latero

#endif
