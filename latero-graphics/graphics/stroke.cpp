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

#include "stroke.h"
#include "../ridge.h"
#include "../grating.h"
#include "texture/vibrotexture.h"
#include "tactileops.h"

namespace latero {
namespace graphics { 

const Stroke::MotionCue Stroke::motion_cue_none(0,"none");
const Stroke::MotionCue Stroke::motion_cue_inherent(1,"inherent");
const Stroke::MotionCue Stroke::motion_cue_superposed(2,"superposed");
const Stroke::MotionCue Stroke::motion_cue_suppressed(3,"suppressed");
const Stroke::MotionCue Stroke::motion_cue_vib(4,"vibration");
const Stroke::MotionCue Stroke::motion_cue_blend(5,"blend");

Stroke::Stroke(const latero::Tactograph *dev) :
	profile_(Ridge::Create(1)),
	intensity_(1.0),
	vib_(Oscillator::Create(50,1,false)),
	gutterEnable_(false),
    motionVel_(50),
    motionCue_(motion_cue_none),
    superposedMotionRatio_(0.2),
	motionVib_(Oscillator::Create(50,1,true)),
	minMotionWidth_(10),
	blendMotionValue_(0.5),
    dotEnable_(false),
	dev_(dev)
{
	motionCueSet_.Append(motion_cue_none);
	motionCueSet_.Append(motion_cue_inherent);
	motionCueSet_.Append(motion_cue_superposed);
	motionCueSet_.Append(motion_cue_suppressed);
	motionCueSet_.Append(motion_cue_vib);
	motionCueSet_.Append(motion_cue_blend);

	SetWidth(10.0);

	texture_ = Grating::Create("mm");
	texture_->SetEnable(false);
	texture_->SetRidgeSize(2);
	texture_->SetRegularizable(true);
	texture_->SetVelUnitsHz(false);

	motionTexture_ = Grating::Create("mm");
	motionTexture_->SetRidgeSize(2);
	motionTexture_->SetGapSize(30);
	motionTexture_->SetRegularizable(true);
	motionTexture_->SetVelUnitsHz(false);

	AddModifiableChild(profile_);
	AddModifiableChild(texture_);
	AddModifiableChild(motionTexture_);
	AddModifiableChild(vib_);
	AddModifiableChild(motionVib_);

	UpdateMotionCue_();
}



double Stroke::Render_(const StrokeState &state, double background, double motionCueAlpha, double invertVib, double overlayValue, double overlayAlpha)
{
	// TODO: move intensity to Pattern
	if (state.dWidth > GetEffectiveWidth_()/2)
		return intensity_ * SuperposeMotion_(background, state, false, motionCueAlpha);

	bool oversized = (width_<minMotionWidth_);

	if (dotEnable_ && dots_)
	{
		// todo: make dots share stroke profile
		double stroke = dots_->Render_(state.actuatorState);
		//stroke = TactileOps::Blend(stroke, overlayValue, overlayAlpha);

		// Consider everything as a background for the motion cue. That makes things much simpler
		// and there's no point in introducing complications since a cue's endpoints are often out
		// of the dots anyway.
		double deflection = (stroke <= 0) ? background : stroke;
		deflection = SuperposeMotion_(deflection, state, true, motionCueAlpha);
		return intensity_ * deflection;
	}
	else
	{
		double stroke = RenderTexture_(state, invertVib);
		stroke = TactileOps::Blend(stroke, overlayValue, overlayAlpha);

		double profile = RenderProfile_(state); // todo: includes latitudinal texture

		if (oversized)
		{
			double deflection = TactileOps::Blend(background, stroke, profile);
			deflection = SuperposeMotion_(deflection, state, true, motionCueAlpha);
			return intensity_ * deflection;
		}
		else
		{
			double deflection = SuperposeMotion_(stroke, state, false, motionCueAlpha);
			deflection = TactileOps::Blend(background, deflection, profile);
			return intensity_ * deflection;
		}
	}
}



double Stroke::SuperposeMotion_(double background, const StrokeState &state, bool rblend, double motionCueAlpha)
{
	//return background;

	double extendedWidth = GetEffectiveWidth_();
	if ((extendedWidth == 0) || (state.dWidth > extendedWidth/2))
	{	
		if (motionCue_ == motion_cue_superposed)
			return background * (1-superposedMotionRatio_);
		else
			return background;
	}
	if (motionCue_ == motion_cue_none)	return background;
	if (motionCue_ == motion_cue_inherent)	return background;
	if (motionCueAlpha == 0) return background;

	// compute profile
	double motionProfile = 1;
	if (rblend)
	{
		motionProfile = GetMotionProfile(state);
		if (motionProfile<=0)	return background;
	}

	double grating = motionCueAlpha*motionProfile * motionTexture_->Render_(state.dLength,state.actuatorState.GetTimeElapsed());

	if (motionCue_ == motion_cue_suppressed)
	{

		double vib = motionVib_->RenderBlend_(state.actuatorState);
		double mask = 1 - vib*grating;
		return TactileOps::Mask(background, mask);
	}
	else if (motionCue_ == motion_cue_superposed)
	{
		double vib = motionVib_->RenderBlend_(state.actuatorState);
		double added = vib * grating;
		double weight = superposedMotionRatio_;
		return TactileOps::WeightedSum(background, added, weight);
	}
	else if (motionCue_ == motion_cue_vib)
	{
		double vib = motionVib_->RenderBlend_(state.actuatorState);
		double blended = vib;
		double alpha = grating;
		return TactileOps::Blend(background, blended, alpha);
	}
	else if (motionCue_ == motion_cue_blend)
	{
		double blended = blendMotionValue_;
		double alpha = grating;
		return TactileOps::Blend(background, blended, alpha);
	}
	else
	{
		return background;
	}
}






double Stroke::GetMotionProfile(const StrokeState &state)
{
	// Note: The motion cue's profile does not include the effect of the intensity of the stroke, nor
	// the latitudinal texture.
	
	double profile = Ridge::GetShape_(state.dWidth, GetEffectiveWidth_(), profile_->GetEdgeWidth_());
	if (width_<minMotionWidth_)
	{
		// Oversized motion cues are faded out linearly as they move past the end points
		// of the stroke. Here we compute the absolute distance to the middle of the stroke, substract
		// half its length and use the result to compute a linear fade.

		double L = texture_->GetLength_();
		if ((state.dLength<0) || (state.dLength>L))
		{
			double d = fabs(state.dLength - L/2) - L/2;
			profile *= fmax(0, 1 - d/(width_/2));
		}
	}
	return profile;
}


double Stroke::RenderShadow_(double dWidth)
{
	double extendedWidth_ = GetEffectiveWidth_();
	if (dWidth > extendedWidth_/2) return 0;
	return Ridge::GetShape_(dWidth,extendedWidth_,profile_->GetEdgeWidth_());
}

double Stroke::RenderTexture_(const StrokeState &state, double invert_vib)
{
	if (state.dWidth > width_/2) return 0;

	// Apply gutter effect
	double gutterEffect = texture_->GetEnable_()?1:0;
	if (gutterEnable_)
	{
		gutterEffect = 1;
		double w = profile_->GetEdgeWidth_();
		if (width_ >= 4*w)
		{
			double gutterRadius = (width_/2.0) - w;
	 		gutterEffect = Ridge::GetShape_(state.dWidth,2*gutterRadius,w);
		}
	}

	// Apply texture
	double txEffect = 1;
	if (texture_->GetEnable_())
		txEffect = texture_->Render_(state.dLength, state.actuatorState.GetTimeElapsed());
	txEffect = 1 - txEffect*gutterEffect;

	// Apply vibration
	double tx = 0;
	if (invert_vib > 0) tx = invert_vib*vib_->RenderBlend_(txEffect,state.actuatorState,true);
	if (invert_vib < 1) tx += (1-invert_vib)*vib_->RenderBlend_(txEffect,state.actuatorState,false);
	return tx;
}

double Stroke::RenderProfile_(const StrokeState &state)
{
	if (width_ == 0) return 0;
	if (state.dWidth > width_/2) return 0;
	return profile_->GetValue_(state.dWidth);
}




void Stroke::SetLength(double L)
{
	texture_->SetLength(L);
	motionTexture_->SetLength(L);
}

double Stroke::GetLength_()
{
	return texture_->GetLength_();
}

void Stroke::SetWidth(double v)
{
	{
		LATERO_GRAPHICS_GUARD;
		SetLastModified_();
		width_ = v;
		profile_->SetWidth_(v);
		if (dots_) dots_->SetThickness(v);
	}
	signal_width_changed_();
}

void Stroke::SetMotionCue(std::string label)
{
	MotionCue *op = motionCueSet_.Get(label);
	if (op) SetMotionCue(*op);
}

void Stroke::SetMotionCue(const MotionCue &mode)
{
	{
		LATERO_GRAPHICS_GUARD;
		motionCue_ = mode;
		SetLastModified_();
		UpdateMotionCue_();
	}
	signal_width_changed_();
}

void Stroke::UpdateMotionCue_()
{
	if (motionCue_ == motion_cue_none)
	{
		if (dots_) dots_->SetVel(0);
		texture_->SetVel(0);
		motionTexture_->SetVel(0);
		motionVib_->SetEnable(false);
	}
	else if (motionCue_ == motion_cue_inherent)
	{
		if (dots_) dots_->SetVel(motionVel_);
		texture_->SetVel(motionVel_);
		motionTexture_->SetVel(0);
		motionVib_->SetEnable(false);
	}
	else if (motionCue_ == motion_cue_superposed)
	{
		if (dots_) dots_->SetVel(0);
		texture_->SetVel(0);
		motionTexture_->SetVel(motionVel_);
		motionVib_->SetEnable(false);
	}
	else if (motionCue_ == motion_cue_suppressed)
	{
		if (dots_) dots_->SetVel(0);
		texture_->SetVel(0);
		motionTexture_->SetVel(motionVel_);
		motionVib_->SetEnable(false);
	}
	else if (motionCue_ == motion_cue_vib)
	{
		if (dots_) dots_->SetVel(0);
		texture_->SetVel(0);
		motionTexture_->SetVel(motionVel_);
		motionVib_->SetEnable(true);
	}
	else if (motionCue_ == motion_cue_blend)
	{
		if (dots_) dots_->SetVel(0);
		texture_->SetVel(0);
		motionTexture_->SetVel(motionVel_);
		motionVib_->SetEnable(false);
	}

}

void Stroke::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("stroke");

	// basic properties
	node.AddChild("width", GetWidth(), units::mm);
	node.AddChild("intensity", GetIntensity(), units::percent);
	profile_->AppendXML(node);

	// fill properties
	XMLOutputNode fillnode = node.AddChild("fill");
	texture_->AppendXML(fillnode);
	vib_->AppendXML(fillnode);
	fillnode.AddChild("gutter", GetGutterEnable());

	// motion properties
	XMLOutputNode motionnode = node.AddChild("motion");
	motionnode.AddChild("cue", GetMotionCue().label);
	motionnode.AddChild("velocity", GetMotionVelocity(), units::mm_per_sec);
	motionnode.AddChild("superposed_ratio", GetSuperposedMotionRatio(), units::percent);
	motionnode.AddChild("blend_value", GetBlendMotionValue(), units::percent);
	motionnode.AddChild("min_width", GetMinMotionWidth(), units::mm);
	motionTexture_->AppendXML(motionnode);
	motionVib_->AppendXML(motionnode);

	XMLOutputNode dotnode = node.AddChild("dots");
	dotnode.AddChild("enable", GetDotEnable());
	if (dots_) dots_->AppendXML(dotnode);
}

void Stroke::LoadXML(const XMLInputNode &root)
{
	//printf("Stroke::LoadXML\n");
	XMLInputNode strokenode = root.GetChild("stroke");
	if (strokenode)
	{

		//printf("Stroke::LoadXML: found stroke node\n");

		// dots
		XMLInputNode dotnode = strokenode.GetChild("dots");
		if (dotnode)
		{
			XMLInputNode node = dotnode.GetChild("enable");
			if (node) SetDotEnable(node.GetBool());

			if (dots_) dots_->LoadXML(dotnode);
		}

		// basic properties
		XMLInputNode node = strokenode.GetChild("width");
		if (node) SetWidth(node.GetFloat());

	 	node = strokenode.GetChild("intensity");
		if (node) SetIntensity(node.GetFloat());

		profile_->LoadXML(strokenode);

		// fill properties
		XMLInputNode fillnode = strokenode.GetChild("fill");
		if (fillnode)
		{
			//printf("Stroke::LoadXML: found fill node\n");

			texture_->LoadXML(fillnode);
			vib_->LoadXML(fillnode);
		
			node = fillnode.GetChild("gutter");
			if (node) SetGutterEnable(node.GetBool());
		}

		// motion properties
		XMLInputNode motionnode = strokenode.GetChild("motion");
		if (motionnode)
		{
			node = motionnode.GetChild("cue");
			if (node) SetMotionCue(node.GetString());

			node = motionnode.GetChild("velocity");
			if (node) SetMotionVelocity(node.GetFloat());

			node = motionnode.GetChild("superposed_ratio");
			if (node) SetSuperposedMotionRatio(node.GetFloat());

			node = motionnode.GetChild("blend_value");
			if (node) SetBlendMotionValue(node.GetFloat());

			node = motionnode.GetChild("min_width");
			if (node) SetMinMotionWidth(node.GetFloat());

			motionTexture_->LoadXML(motionnode);
			motionVib_->LoadXML(motionnode);
		}
	}
}

void Stroke::SetDots(DotPatternPtr dots)
{
	LATERO_GRAPHICS_GUARD;
	if (dots_) RemoveModifiableChild_(dots_);
	dots_ = dots;
	dots_->SetProfile(profile_);
	dots_->SetOscillator(vib_);
	AddModifiableChild_(dots_);
	SetLastModified_();
}

} // namespace graphics
} // namespace latero
