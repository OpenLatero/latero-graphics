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

#include "line.h"
#include <math.h>
#include "linewidget.h"
#include "dotpattern.h"
#include "../grating.h"
#include "../vector.h"


namespace latero {
namespace graphics { 

Line::Line(const latero::Tactograph *dev, const XMLInputNode &node) :
	Pattern(dev), stroke_(Stroke::Create(dev)),
	dots_(DottedLine::Create(dev,Point(0,0), dev->GetSurfaceSize()))
{
	SetEndPoints(Point(0,0), dev->GetSurfaceSize());
	stroke_->SetDots(dots_);
	AddModifiableChild(stroke_);
	LoadXML(node);
}

Line::Line(const latero::Tactograph *dev, Point p1, Point p2) :
	Pattern(dev), stroke_(Stroke::Create(dev)),
	dots_(DottedLine::Create(dev,p1,p2))
{
	SetEndPoints(p1,p2);
	stroke_->SetDots(dots_);
	AddModifiableChild(stroke_);
}

Line::~Line()
{
}

void Line::LoadXML(const XMLInputNode &root)
{
	stroke_->LoadXML(root);

	XMLInputNode node = root.GetChild("start_point");
	if (node) SetStartPoint(node.GetPoint());

	node = root.GetChild("end_point");
	if (node) SetEndPoint(node.GetPoint());
}

void Line::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Line");

	node.AddChild("start_point", GetStartPoint(), units::mm);
	node.AddChild("end_point", GetEndPoint(), units::mm);

	stroke_->AppendXML(node);
}


double Line::DoRender_(const ActuatorState &state)
{
	StrokeState strokeState(state);
	GetOrthoDistancesToLineSegment(state.pos, p1_, p2_, strokeState.dLength, strokeState.dWidth);
	return stroke_->Render_(strokeState);
}

double Line::DoRenderShadow_(const ActuatorState &state)
{
	double dWidth = GetDistanceToLineSegment(state.pos, p1_, p2_);
	return stroke_->RenderShadow_(dWidth);
}


Cairo::RefPtr<Cairo::Pattern> Line::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t)
{
	if (stroke_->GetDotEnable())
		return dots_->GetDrawingPattern(cr,t);

	cr->push_group();

	Point p1 = GetStartPoint();
	Point p2 = GetEndPoint();
	double value = stroke_->GetIntensity();
	double thickness = stroke_->GetWidth();

	cr->push_group();
	cr->set_source_rgba(0, 0, 0, value);
	cr->set_line_width(thickness);
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);
	cr->move_to(p1.x, p1.y);
	cr->line_to(p2.x, p2.y);
	cr->stroke();	
	Cairo::RefPtr<Cairo::Pattern> strokePattern = cr->pop_group();

	if (stroke_->GetOscillator()->GetEnable())
		strokePattern = stroke_->GetOscillator()->GetBlendPattern(strokePattern,cr);
	cr->set_source(strokePattern);
	cr->paint();

	return cr->pop_group();
}



Gtk::Widget *Line::CreateWidget(PatternPtr ptr)
{
	LinePtr gen = boost::dynamic_pointer_cast<Line>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new LineWidget(gen);
}

std::string Line::GetName()
{
	return "Line";
}

void Line::SetEndPoints(Point p1, Point p2)
{
	LATERO_GRAPHICS_LOCK;
	p1_ = p1;
	p2_ = p2;
	LATERO_GRAPHICS_UNLOCK;
	dots_->SetEndPoints(GetStartPoint(),GetEndPoint());
	Precompute();
}

void Line::Precompute()
{
	LATERO_GRAPHICS_GUARD;
	length_ = (p2_-p1_).Norm();
	unitVector_ = (p2_-p1_)/length_;
	stroke_->SetLength(length_); // TODO: + width?
}

bool Line::UpdateVisible_(const State *state)
{
	double d = GetDistanceToLineSegment(state->pos, p1_, p2_);
	bool visible = (d <= (stroke_->GetEffectiveWidth_()/2 + Dev()->GetRadius()));
	SetVisible_(visible);
	return visible;
}

} // namespace graphics
} // namespace latero



