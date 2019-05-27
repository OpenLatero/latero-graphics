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

#ifndef LATERO_GRAPHICS_PLANAR_TEXTURE
#define LATERO_GRAPHICS_PLANAR_TEXTURE

#include "texturefwd.h"
#include "../../modifiable.h"
#include <latero/tactograph.h>
#include "../../xml.h"
#include "../../oscillator.h"
#include <cairomm/context.h>
#include <gtkmm/widget.h>
#include "../../state.h"
#include "../pattern.h"

namespace latero {
namespace graphics { 

class Texture : virtual public Pattern
{
public:
    constexpr static const double amplitude_default = 1.0;
	static const bool invert_default = false;
    constexpr static const double vibEnable_default = false;
	static const bool tdCentric_default = false;
	static const Point tdCentricPos_default;
	static const bool motionEnable_default = false;
	static const Point motionVector_default;

	static TexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node);
	static TexturePtr Create(const latero::Tactograph *dev, std::string xmlFile);
	void Save(std::string xmlFile);

	virtual ~Texture();

	/** returns a deflections between 0-right and 1-left */
	virtual double DoRender_(const ActuatorState &state)
	{
		const ActuatorState *s = &state;

		ActuatorState dummy;
		if (motionEnable_ || tdCentric_)
		{
			// avoid making this copy...
			dummy = state;
			s = &dummy;

			if (tdCentric_) ApplyTDCentric(dummy);
			if (motionEnable_ && !motionOverride_) ApplyMotion(dummy);
		}

		double rv = NormRender_(*s);
		rv = ApplyAmplitude(rv);
		rv = ApplyInvert(rv);
		if (vib_->GetEnable_()) rv = ApplyVibrate(rv,*s);
		return rv;
	}

	virtual double DoRenderShadow_(const ActuatorState &state)
	{ 
		const ActuatorState *s = &state;

		ActuatorState dummy;
		if (motionEnable_ || tdCentric_)
		{
			// avoid making this copy...
			dummy = state;
			s = &dummy;

			if (tdCentric_) ApplyTDCentric(dummy);
			if (motionEnable_ && !motionOverride_) ApplyMotion(dummy);
		}

		double rv = NormRenderShadow_(*s);
		rv = ApplyAmplitude(rv);
		rv = ApplyInvert(rv);
		return rv;
	}

	inline void ApplyMotion(ActuatorState &state)
	{
		state.pos -= motionVector_ * (state.GetTimeElapsed().total_microseconds()/1E6);
	}

	inline void ApplyTDCentric(ActuatorState &state)
	{
		if (!state.mode.sim) state.pos = tdCentricPos_ + state.relativePos;
	}

	inline double ApplyAmplitude(double value)
	{
		return value*amplitude_;
	}

	inline double ApplyInvert(double value)
	{
		return (invert_)?1-value:value;
	}

	inline double ApplyVibrate(double value, const ActuatorState &state)
	{
		return vib_->RenderBlend_(value,state);
	}

	// Render without amplitude and inversion effects
	virtual double NormRender_(const ActuatorState &state) = 0;
	virtual double NormRenderShadow_(const ActuatorState &state) { return NormRender_(state); };

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	/**
	 * Returns a texture pattern to be used as a source in drawing context.
	 * @param mmContext drawing context in units of the virtual surface
	 */
	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t) = 0;

	virtual std::string GetName() { return "Texture"; }

	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr) { return CreateWidget(ptr); }

	virtual void AppendXML(XMLOutputNode &root) const;

	void AppendAmplitude(XMLOutputNode &root) const;
	void AppendIcon(XMLOutputNode &root) const;
	void AppendInvert(XMLOutputNode &root) const;
	void AppendVibration(XMLOutputNode &root) const;
	void AppendTDCentric(XMLOutputNode &root) const;
	void AppendMotion(XMLOutputNode &root) const;

	virtual void LoadXML(const XMLInputNode &node);
	void LoadAmplitude(const XMLInputNode &node);
	void LoadIcon(const XMLInputNode &node);
	void LoadInvert(const XMLInputNode &node);
	void LoadVibration(const XMLInputNode &node);
	void LoadTDCentric(const XMLInputNode &root);
	void LoadMotion(const XMLInputNode &root);

	double GetAmplitude() const 	{ LATERO_GRAPHICS_GUARD; return amplitude_; }
	void SetAmplitude(double v)	{ LATERO_GRAPHICS_GUARD; amplitude_ = v; SetLastModified_(); }

	std::string GetIconFile() const	{ LATERO_GRAPHICS_GUARD; return (iconfile_!="")?iconfile_:GetDefaultIconFile(); }
	void SetIconFile(std::string v)	{ LATERO_GRAPHICS_GUARD; iconfile_ = v; }
	virtual std::string GetDefaultIconFile() const = 0;

	inline bool GetInvert() const { LATERO_GRAPHICS_GUARD; return invert_; }
	inline void SetInvert(bool v) { LATERO_GRAPHICS_GUARD; invert_=v; SetLastModified_(); }

	OscillatorPtr GetOscillator() { return vib_; }

	void SetVibEnable(bool v) { vib_->SetEnable(v); }
	bool GetVibEnable() const { return vib_->GetEnable(); };

	void SetVibAmplitude(double v) { vib_->SetAmplitude(v); SetLastModified_(); }
	double GetVibAmplitude() const { return vib_->GetAmplitude(); };

	void SetVibFreq(double hz) { vib_->SetFreq(hz); }
	double GetVibFreq() const { return vib_->GetFreq(); };

	void SetTDCentric(bool v) { LATERO_GRAPHICS_GUARD; tdCentric_=v; SetLastModified_(); }
	bool GetTDCentric() const { LATERO_GRAPHICS_GUARD; return tdCentric_; }

	void SetTDCentricPos(const Point &v) { LATERO_GRAPHICS_GUARD; tdCentricPos_=v; SetLastModified_(); }
	Point GetTDCentricPos() const { LATERO_GRAPHICS_GUARD; return tdCentricPos_; }

	/**
	 * Motion by translation
	 */
	void SetMotionEnable(bool v) 
		{ LATERO_GRAPHICS_GUARD; motionEnable_=v; SetLastModified_(); OnMotionChange(); }
	bool GetMotionEnable() const 	    { LATERO_GRAPHICS_GUARD; return motionEnable_; }
	void SetMotionDirection(double v)    { LATERO_GRAPHICS_GUARD; motionVector_.SetAngleDegree(v); SetLastModified_(); OnMotionChange(); } 
	double GetMotionDirection() const    { LATERO_GRAPHICS_GUARD; return motionVector_.AngleDegree(); }
	void SetMotionVelocity(double v)     { LATERO_GRAPHICS_GUARD; motionVector_.SetNorm(v); SetLastModified_(); OnMotionChange(); }
	double GetMotionVelocity() const     { LATERO_GRAPHICS_GUARD; return motionVector_.Norm(); }
	virtual void OnMotionChange() {};

	const latero::Tactograph *Dev() { return dev_; }
protected:
	Texture(const latero::Tactograph *dev);

	// TODO: automate amplitude modulation and inversion
	bool invert_;			// invert the output (1-0 instead of 0-1)
	double amplitude_;		// amplitude of the texture, between 0 and 1
	std::string iconfile_;		// path of the image file used as an icon (if specified)
	OscillatorPtr vib_;		// vibration generator
	bool tdCentric_;		// if true the rendering is independent of the user's motion
	Point tdCentricPos_;		// simulated display position when in tdCentric mode

	// motion by translation
	bool motionOverride_; // true if motion is overriden by derived class
	bool motionEnable_;
	Point motionVector_;

	const latero::Tactograph *dev_; 
};

} // namespace graphics
} // namespace latero

#endif

