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

#include "circle.h"
#include <math.h>
#include "circlewidget.h"
#include "stroke.h"
#include "../units.h"
#include "dotpattern.h"
#include "texture/texture.h"
#include "texture/patternops.h"

namespace latero {
namespace graphics { 

Circle::Circle(const latero::Tactograph *dev, const XMLInputNode &node)  :
	ClosedPattern(dev),
	stroke_(Stroke::Create(dev)),
	dots_(DottedCircle::Create(dev,Point(0,0),100))
{

	stroke_->SetDots(dots_);
	SetCenter(Point(0,0));
	SetRadius(100);
	AddModifiableChild(GetStroke());
	LoadXML(node);
	printf("finished constructing circle\n");
}


Circle::Circle(const latero::Tactograph *dev, Point center, double radius) :
	ClosedPattern(dev),
	stroke_(Stroke::Create(dev)),
	dots_(DottedCircle::Create(dev,center,radius))
{
	stroke_->SetDots(dots_);
	SetCenter(center);
	SetRadius(radius);
	AddModifiableChild(GetStroke());
	printf("finished constructing circle\n");
}

void Circle::LoadXML(const XMLInputNode &root)
{
	ClosedPattern::LoadXML(root);
	stroke_->LoadXML(root);

	XMLInputNode node = root.GetChild("center");
	if (node) SetCenter(node.GetPoint());

	node = root.GetChild("radius");
	if (node) SetRadius(node.GetFloat());
}

void Circle::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Circle");

	node.AddChild("center", GetCenter(), units::mm);
	node.AddChild("radius", GetRadius(), units::mm);

	stroke_->AppendXML(node);
	ClosedPattern::AppendXML(node);
}


bool Circle::UpdateVisible_(const State *state)
{
	bool visible = false;
	double d = radius_ - (state->pos - center_).Norm();
	if ((d>0) && GetFillTextureEnable_())
		visible = true;
	else if (fabs(d) <= stroke_->GetEffectiveWidth_()/2 + Dev()->GetRadius())
		visible = true;
	SetVisible_(visible);
	return visible;
}


double Circle::RenderFilling_(double dLength, double dWidth, const ActuatorState &state)
{
	if (!GetFillTextureEnable_() || !GetFillTexture_() || ((state.pos - center_).Norm() > radius_))
	{
		// texture is disabled or doesn't exist, or outside the shape
		return 0;
	}
	else
	{
		// inside shape
		double dFromEdge = dWidth - stroke_->GetWidth_()/2;
		return ClosedPattern::RenderFilling_(dFromEdge, state);
	}
}

double Circle::DoRender_(const ActuatorState &state)
{
	StrokeState strokeState(state);
	GetDistances_(state.pos, strokeState.dLength, strokeState.dWidth);
	double filling = RenderFilling_(strokeState.dLength, strokeState.dWidth, state);
	return stroke_->Render_(strokeState, filling);
}

double Circle::DoRenderShadow_(const ActuatorState &state)
{
	Point delta = state.pos - center_;
	double d = delta.Norm();
	if (GetFillTextureEnable_() && (d < radius_))
		return 1;

	double dWidth = fabs(d-radius_);
	return stroke_->RenderShadow_(dWidth);
}


Cairo::RefPtr<Cairo::Pattern> Circle::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t)
{
	cr->push_group();


	if (GetRadius()<stroke_->GetWidth()/2) return cr->pop_group();

	double radius = GetRadius();
	Point center = GetCenter();
	double value = stroke_->GetIntensity();
	double thickness = stroke_->GetWidth();

	if (GetFillTextureEnable())
	{
		double r = radius;
		if (GetFillTextureGap()>=0)
			 r -= thickness/2 + GetFillTextureGap();
				
		cr->push_group();
		cr->set_source_rgba(0,0,0,value);
		cr->set_line_width(0);
		cr->arc(center.x, center.y, r, 0, 2*M_PI);
		cr->fill();
		Cairo::RefPtr<Cairo::Pattern> fillMask = cr->pop_group();

		Cairo::RefPtr<Cairo::Pattern> txMask = GetFillTexture()->GetDrawingPattern(cr,t);

		Cairo::RefPtr<Cairo::Pattern> fill = PatternOps::Multiply(cr, fillMask, txMask);
		cr->set_source(fill);
		cr->paint();
	}

	if (stroke_->GetDotEnable())
	{
		cr->set_source(dots_->GetDrawingPattern(cr,t));
		cr->paint();
	}
	else
	{
		cr->push_group();
		cr->set_source_rgba(0,0,0,value);
		cr->set_line_width(thickness);
		cr->arc(center.x, center.y, radius, 0, 2*M_PI);
		cr->stroke();
		Cairo::RefPtr<Cairo::Pattern> strokePattern = cr->pop_group();

		if (stroke_->GetOscillator()->GetEnable())
			strokePattern = stroke_->GetOscillator()->GetBlendPattern(strokePattern,cr);
		cr->set_source(strokePattern);
		cr->paint();
	}
	return cr->pop_group();
}


Gtk::Widget *Circle::CreateWidget(PatternPtr ptr)
{
	CirclePtr gen = boost::dynamic_pointer_cast<Circle>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new CircleWidget(gen);
}

std::string Circle::GetName()
{
	return "Circle";
}

void Circle::SetRadius(float v)
{
	LATERO_GRAPHICS_GUARD;
	radius_ = fmax(0,v);
	stroke_->SetLength(GetCircumference_());
	dots_->SetRadius(radius_);
	SetLastModified_();
}

void Circle::SetCenter(const Point &v)  { LATERO_GRAPHICS_GUARD; center_=v; SetLastModified_(); dots_->SetCenter(v); }

} // namespace graphics
} // namespace latero


