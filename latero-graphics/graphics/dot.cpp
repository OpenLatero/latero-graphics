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

#include "dot.h"
#include "dotwidget.h"
#include "../ridge.h"
#include "../units.h"

namespace latero {
namespace graphics { 

void Dot::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Dot");

	// todo: invertVib is NOT taken into account
	node.AddChild("dot", GetPos(), units::mm);
	AppendHeight(node);
	AppendDotRadius(node);
	AppendVib(node);
	AppendProfile(node);
}

void Dot::AppendHeight(XMLOutputNode &node) const 	{ node.AddChild("height", GetHeight(), units::percent); }
void Dot::AppendDotRadius(XMLOutputNode &node) const 	{ node.AddChild("dot_radius", GetDotRadius(), units::mm); }
void Dot::AppendVib(XMLOutputNode &node) const 	{ vib_->AppendXML(node); }
void Dot::AppendProfile(XMLOutputNode &node) const 	{ profile_->AppendXML(node); }


Dot::Dot(const latero::Tactograph *dev, const XMLInputNode &root) :
    Pattern(dev),
	pos_(0,0),
	invertVib_(false),
	height_(1)
{
	SetProfile(Ridge::Create());
	SetOscillator(Oscillator::Create(50,1,false));
	SetDotRadius(100);
	profile_->SetEdgeWidth(0.2);
	LoadXML(root);
}

Dot::Dot(const latero::Tactograph *dev, const Point &pos, double radius, double height, bool invertVib) :
	Pattern(dev),
	pos_(pos),
	invertVib_(invertVib),
	height_(height)
{
	SetProfile(Ridge::Create());
	SetOscillator(Oscillator::Create(50,1,false));
	SetDotRadius(radius);
	profile_->SetEdgeWidth(0.2);
}

void Dot::LoadXML(const XMLInputNode &root)
{
	LoadVib(root);
	LoadProfile(root);
	LoadPoints(root);
	LoadHeight(root);
	LoadDotRadius(root);
}

void Dot::LoadPoints(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("dot");
	if (node) SetPos(node.GetPoint());
}

void Dot::LoadProfile(const XMLInputNode &root)
{
	profile_->LoadXML(root);
}

void Dot::LoadVib(const XMLInputNode &root)
{
	vib_->LoadXML(root);
}


void Dot::LoadHeight(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("height");
	if (node) SetHeight(node.GetFloat());
}

void Dot::LoadDotRadius(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("dot_radius");
	if (node) SetDotRadius(node.GetFloat());
}


Cairo::RefPtr<Cairo::Pattern> Dot::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t)
{
	cr->push_group();
	double radius = GetDotRadius();
	Point pos = GetPos();

	cr->set_source_rgba(0,0,0,GetHeight());

	cr->save();
	cr->translate(pos.x,pos.y);
	cr->scale(radius,radius);
	cr->arc(0.0, 0.0, 1.0, 0, 2*M_PI);
	cr->restore();
	cr->fill();

	return cr->pop_group();
}


double Dot::DoRender_(const ActuatorState &state)
{
	float rv = 0.0;
	Point delta = pos_ - state.pos;
	float d2 = delta.NormSquared();
	if (d2 < dotRadius_*dotRadius_)
	{	
		float d = sqrt(d2);
		rv = height_ * profile_->GetValue_(d);
		if (rv==0) return rv;

		double vib=1;
		if (invertVib_) 
			vib = vib_->RenderBlend_(state, Oscillator::mode_invert_enable);
		else
			vib = vib_->RenderBlend_(state);
			
		return rv * vib;
	}
	return rv;
}

double Dot::DoRenderShadow_(const ActuatorState &state)
{
	Point delta = pos_ - state.pos;
	float d2 = delta.NormSquared();
	if (d2 < dotRadius_*dotRadius_)
		return profile_->GetShape_(sqrt(d2));
	return 0;
}



Gtk::Widget *Dot::CreateWidget(PatternPtr ptr)
{
	DotPtr gen = boost::dynamic_pointer_cast<Dot>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new DotWidget(gen);
}

void Dot::SetDotRadius(float v)
{
	LATERO_GRAPHICS_GUARD; 
	dotRadius_ = v; 
	profile_->SetWidth(2*v);
	SetLastModified_();
}

void Dot::SetOscillator(OscillatorPtr p)
{
	LATERO_GRAPHICS_GUARD;
	if (vib_) RemoveModifiableChild_(vib_);
	vib_ = p;
	AddModifiableChild_(vib_);
	SetLastModified_(); 
}
	
void Dot::SetProfile(RidgePtr v)
{
	LATERO_GRAPHICS_GUARD;
	if (profile_) RemoveModifiableChild_(profile_);
	profile_ = v;
	AddModifiableChild_(profile_);
	SetLastModified_();
}

bool Dot::UpdateVisible_(const State *state)
{
	bool visible = false;
	double d = (state->pos - pos_).Norm();
	visible = (d <= dotRadius_ + Dev()->GetRadius());
	SetVisible_(visible);
	return visible;
}

} // namespace graphics
} // namespace latero



