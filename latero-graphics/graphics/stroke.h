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

#ifndef LATERO_GRAPHICS_PLANAR_STROKE_H
#define LATERO_GRAPHICS_PLANAR_STROKE_H

#include "strokefwd.h"
#include "../modifiable.h"
#include <latero/tactograph.h>
#include "../gratingfwd.h"
#include "texture/vibrotexturefwd.h"
#include "../ridgefwd.h"
#include "../oscillator.h"
#include "../namedid.h"
#include <sigc++/signal.h>
#include "dotpattern.h"

namespace latero {
namespace graphics { 

class StrokeState
{
public:
	StrokeState(const ActuatorState &s) : actuatorState(s) {}

	/**
	 * Longitudinal distance along the length of the stroke. The length should grow from minus infinity
	 * and reach the starting point of the stroke at 0. This distance will therefore be negative in the rounded
	 * portion of the point that extends beyond the starting point.
	 */
	double dLength;

	/**
	 * Latitudinal distance across the stroke. This is the minimal distance to a point on the stroke whithin the
	 * defined segment. 
	 */
	double dWidth;

	/**
	 * State of the rendering system.
	 */
	const ActuatorState &actuatorState;
};

class Stroke : public Modifiable
{
public:
	typedef NamedID MotionCue;
	typedef NamedIDSet MotionCueSet;
	static const MotionCue motion_cue_none, motion_cue_inherent, motion_cue_superposed, motion_cue_suppressed, motion_cue_vib, motion_cue_blend;

	static StrokePtr Create(const latero::Tactograph *dev) { return StrokePtr(new Stroke(dev)); };
	virtual ~Stroke() {};

	void LoadXML(const XMLInputNode &root);
	void AppendXML(XMLOutputNode &root) const;

	/**
	 * @param dLength Longitudinal distance along the length of the stroke. The length should grow from minus infinity
	 * and reach the starting point of the stroke at 0. This distance will therefore be negative in the rounded portion of
	 * the point that extends beyond the starting point.
	 * @param dWidth Latitudinal distance across the stroke. This is the minimal distance to a point on the stroke whithin the
	 * defined segment. 
	 */
	double Render_(const StrokeState &state, double background=0, double motionCueAlpha = 1, double invertVib = 0, double overlayValue = 1, double overlayAlpha = 0);	
	double RenderShadow_(double dWidth);
	
	double RenderProfile_(const StrokeState &state);
	double RenderTexture_(const StrokeState &state, double invert_vib);

	/** Superpose a motion cue over a background deflection. */
	double SuperposeMotion_(double deflection, const StrokeState &state, bool rblend, double motionCueAlpha);



	double GetLength_();
	void SetLength(double L);

	/***********************************************
	 * PROFILE
	 ***********************************************/

	/** Width of the line (mm). */
	inline double GetWidth_() const { return width_; }
	inline double GetWidth() const  { LATERO_GRAPHICS_GUARD; return GetWidth_(); }
	void SetWidth(double v);

	/** Width of the line, including oversized motion cues (mm) */
	double GetEffectiveWidth_() const 
	{
		if ((motionCue_ == motion_cue_superposed) || (motionCue_ == motion_cue_vib) || (motionCue_ == motion_cue_suppressed) || (motionCue_ == motion_cue_blend))
			return fmax(width_,minMotionWidth_);
		else
			return width_;
	}
	inline double GetEffectiveWidth() const  { LATERO_GRAPHICS_GUARD; return GetEffectiveWidth_(); }

	/** Intensity of the line (0-1). */
	inline double GetIntensity_() const { return intensity_; }
	inline double GetIntensity() const  { LATERO_GRAPHICS_GUARD; return GetIntensity_(); }
	inline void SetIntensity_(double v) { intensity_ = v; SetLastModified_(); }
	inline void SetIntensity(double v) { LATERO_GRAPHICS_GUARD; SetIntensity_(v); }

	/** Controls the lateral profile of the stroke (across). */
	RidgePtr GetProfile() { return profile_; }

	/***********************************************
	 * FILL
	 ***********************************************/

	/** Controls the vibration of the entire stroke. */
	OscillatorPtr GetOscillator() { return vib_; }

	/** Controls the longitudinal texture. */
	GratingPtr GetTexture() { return texture_; }

	/** 
	 * Determines whether the texture apply across the entire profile or only as 
 	 * a "gutter" in the middle.
	 */
	inline bool GetGutterEnable() const { LATERO_GRAPHICS_GUARD; return GetGutterEnable_(); }
	inline bool GetGutterEnable_() const { return gutterEnable_; }
	inline void SetGutterEnable(bool v) { LATERO_GRAPHICS_GUARD; SetGutterEnable_(v); }
	inline void SetGutterEnable_(bool v) { gutterEnable_=v; SetLastModified_(); }


	/***********************************************
	 * MOTION CUE
	 ***********************************************/

	/** 
	 * Selects the motion cue.
	 */
	MotionCueSet GetMotionCues() const { return motionCueSet_; };
	MotionCue GetMotionCue() const { LATERO_GRAPHICS_GUARD; return motionCue_; }
	void SetMotionCue(const MotionCue &mode);
	void SetMotionCue(std::string label);

	/**
	 * Select the velocity in mm/s.
	 */
	inline double GetMotionVelocity_() const { return motionVel_; }
	inline double GetMotionVelocity() const  { LATERO_GRAPHICS_GUARD; return GetMotionVelocity_(); }
	inline void SetMotionVelocity(double v) { LATERO_GRAPHICS_GUARD; motionVel_ = v; SetLastModified_(); UpdateMotionCue_(); }

	/**
	 * Set the ratio of the motion cue in "superposed" mode.
	 */
	inline double GetSuperposedMotionRatio_() const { return superposedMotionRatio_; }
	inline double GetSuperposedMotionRatio() const  { LATERO_GRAPHICS_GUARD; return GetSuperposedMotionRatio_(); }
	inline void SetSuperposedMotionRatio(double v) { LATERO_GRAPHICS_GUARD; superposedMotionRatio_ = v; SetLastModified_(); }	

	/**
	 * Set the value to blend to in "blend" mode
	 */
	inline double GetBlendMotionValue_() const { return blendMotionValue_; }
	inline double GetBlendMotionValue() const  { LATERO_GRAPHICS_GUARD; return GetBlendMotionValue_(); }
	inline void SetBlendMotionValue(double v) { LATERO_GRAPHICS_GUARD; blendMotionValue_ = v; SetLastModified_(); }	


	/**
	 * Controls the motion cue grating (when applicable).
	 */	
	GratingPtr GetMotionTexture() { return motionTexture_; }

	/**
	 * Controls the oscillation in "vib" mode
	 */	
	OscillatorPtr GetMotionOscillator() { return motionVib_; }

	/**
	 * Set the minimum thickness of motion effects (when applicable).
	 */
	inline double GetMinMotionWidth_() const { return minMotionWidth_; }
	inline double GetMinMotionWidth() const  { LATERO_GRAPHICS_GUARD; return GetMinMotionWidth_(); }
	inline void SetMinMotionWidth(double v) { {LATERO_GRAPHICS_GUARD; minMotionWidth_ = v; SetLastModified_();} signal_width_changed_(); }	

	/** signals... activated unlocked... */
	// applies to both width and effective width
	sigc::signal<void> signal_width_changed() { return signal_width_changed_; };


	/***********************************************
	 * Dotted Style
	 ***********************************************/

	inline void SetDotEnable(bool v) { LATERO_GRAPHICS_GUARD; dotEnable_=v; SetLastModified_(); }
	inline bool GetDotEnable() const { LATERO_GRAPHICS_GUARD; return GetDotEnable_(); }
	inline bool GetDotEnable_() const { return dotEnable_; }

	void SetDots(DotPatternPtr dots);
	DotPatternPtr GetDots() const { LATERO_GRAPHICS_GUARD; return dots_; }

protected:
	Stroke(const latero::Tactograph *dev);
	void UpdateMotionCue_();

	/** @return the profile of the motion cue, without the intensity */
	double GetMotionProfile(const StrokeState &state);

	sigc::signal<void> signal_width_changed_;

	// basic properties
	RidgePtr profile_; 	// shape of the ridge (smooth, textured, etc)
	double width_; 		// width of the stroke
	double intensity_; 	// between 0 and 1

	// fill properties
	OscillatorPtr vib_;	// vibration of the entire pattern
	GratingPtr texture_;	// fill texture
	bool gutterEnable_;	// add a border to the edge of the stroke, when possible

	// motion properties
	double motionVel_;		// velocity in mm/s	
	MotionCue motionCue_;		// selects the motion cue
	MotionCueSet motionCueSet_;
	double superposedMotionRatio_;	// ratio of the motion cue when in "superposed" mode
	GratingPtr motionTexture_; 	// controls the grating of the motion cue (except for "inherent" motion
	OscillatorPtr motionVib_;	// vibration of the motion cues
	double minMotionWidth_;		// minimal width of the motion effects
	double blendMotionValue_;	// value to blend to

	// dotted props
	bool dotEnable_;
	DotPatternPtr dots_;

	const latero::Tactograph *dev_;
};

} // namespace graphics
} // namespace latero

#endif
