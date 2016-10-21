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

#include "dots.h"
#include "../ridge.h"
#include "../units.h"
#include "../vector.h"
#include "dotswidget.h"

namespace latero {
namespace graphics { 



Dots::Dots(const latero::Tactograph *dev, const XMLInputNode &root) :
	Pattern(dev), Group(dev),
	height_(1),
	vib_(Oscillator::Create(50,1,false))
{
	SetOperation(Group::op_add);
	SetProfile(Ridge::Create());
	SetDotRadius(100);
	profile_->SetEdgeWidth(0.2);
	AddModifiableChild(profile_);
	LoadXML(root);
}

Dots::Dots(const latero::Tactograph *dev, double radius, double height) :
	Pattern(dev), Group(dev),
	height_(height),
	vib_(Oscillator::Create(50,1,false))
{
	SetOperation(Group::op_add);
	SetProfile(Ridge::Create());
	SetDotRadius(radius);
	profile_->SetEdgeWidth(0.2);
	AddModifiableChild(profile_);
}

Dots::Dots(const latero::Tactograph *dev, const std::vector<Point> &points, double radius, double height) :
	Pattern(dev), Group(dev),
	height_(height),
	vib_(Oscillator::Create(50,1,false))
{
	SetOperation(Group::op_add);
	SetProfile(Ridge::Create());
	SetDotRadius(radius);
	profile_->SetEdgeWidth(0.2);
	AddModifiableChild(profile_);
	InsertPoints(points);
}


void Dots::LoadXML(const XMLInputNode &root)
{
	LoadVib(root);
	LoadProfile(root);
	LoadPoints(root);
	LoadHeight(root);
	LoadDotRadius(root);
	Group::LoadOperation(root);
}

void Dots::LoadPoints(const XMLInputNode &root)
{
	std::vector<XMLInputNode> list = root.GetChildren("dot");
	std::vector<Point> points;
	for(unsigned int i=0; i<list.size(); ++i)
		points.push_back(list[i].GetPoint());
	SetPoints(points);
}

void Dots::LoadProfile(const XMLInputNode &root)
{
	profile_->LoadXML(root);
}

void Dots::LoadVib(const XMLInputNode &root)
{
	vib_->LoadXML(root);
}


void Dots::LoadHeight(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("height");
	if (node) SetHeight(node.GetFloat());
}

void Dots::LoadDotRadius(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("dot_radius");
	if (node) SetDotRadius(node.GetFloat());
}


void Dots::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Dots");

	// todo: invertVib is NOT taken into account
	std::vector<DotPtr> dotSet = GetDots();
	for (unsigned int i=0; i<dotSet.size();++i)
		node.AddChild("dot", dotSet[i]->GetPos(), units::mm);

	AppendHeight(node);
	AppendDotRadius(node);
	AppendVib(node);
	AppendProfile(node);
	Group::AppendOperation(node);
}

void Dots::AppendHeight(XMLOutputNode &node) const 	{ node.AddChild("height", GetHeight(), units::percent); }
void Dots::AppendDotRadius(XMLOutputNode &node) const 	{ node.AddChild("dot_radius", GetDotRadius(), units::mm); }
void Dots::AppendVib(XMLOutputNode &node) const 	{ vib_->AppendXML(node); }
void Dots::AppendProfile(XMLOutputNode &node) const 	{ profile_->AppendXML(node); }

void Dots::InsertPoints(const std::vector<Point> &points)
{
	for (unsigned int i=0; i<points.size(); ++i)
		InsertPoint(points[i]);
	SetLastModified();
}

void Dots::SetPoints(const std::vector<Point> &points)
{
	ClearPoints();
	InsertPoints(points);
}

Gtk::Widget *Dots::CreateWidget(PatternPtr ptr)
{
	DotsPtr gen = boost::dynamic_pointer_cast<Dots>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new DotsWidget(gen);
}

void Dots::SetDotRadius(float v)
{
	LATERO_GRAPHICS_GUARD; 
	dotRadius_ = v; 
	profile_->SetWidth(2*v);
	std::vector<DotPtr> dotSet = GetDots();
	for (unsigned int i=0; i<dotSet.size(); ++i)
		dotSet[i]->SetDotRadius(v);
	SetLastModified_();
}

float Dots::GetHeight() const	 { LATERO_GRAPHICS_GUARD; return height_; }
void Dots::SetHeight(float v)
{
	LATERO_GRAPHICS_GUARD; 
	height_=v; 
	std::vector<DotPtr> dotSet = GetDots();
	for (unsigned int i=0; i<dotSet.size(); ++i)
		dotSet[i]->SetHeight(v);
	SetLastModified_();
}

void Dots::SetPoint(int i, const Point &p)
{
	LATERO_GRAPHICS_GUARD; 
	std::vector<DotPtr> dotSet = GetDots();
	if (i < (int)dotSet.size())
		dotSet[i]->SetPos(p);
	SetLastModified_();
}

Point Dots::GetPoint(int i) const
{
	LATERO_GRAPHICS_GUARD;
	std::vector<DotPtr> dotSet = GetDots(); 
	if (i < (int)dotSet.size())
		return dotSet[i]->GetPos();
    return Point(0,0); // todo
}


void Dots::InsertPoint(const Point &p, bool invertVib)
{
	DotPtr dot = Dot::Create(Dev(), p, GetDotRadius(), GetHeight(), invertVib);
	dot->SetProfile(GetProfile());
	dot->SetOscillator(GetOscillator());
	Group::InsertPattern(dot);
}

std::vector<Point> Dots::GetPoints() const
{
	LATERO_GRAPHICS_GUARD;
	std::vector<Point> rv;
	std::vector<DotPtr> dotSet = GetDots(); 
	for (unsigned int i=0; i<dotSet.size(); ++i)
		rv.push_back(dotSet[i]->GetPos());
	return rv;
}

void Dots::SetProfile(RidgePtr v)
{
	LATERO_GRAPHICS_GUARD; 
	if (profile_) RemoveModifiableChild(profile_);
	std::vector<DotPtr> dotSet = GetDots(); 
	for (unsigned int i=0; i<dotSet.size(); ++i)
		dotSet[i]->SetProfile(v);
	profile_ = v;
	AddModifiableChild(profile_);
	SetLastModified_();
}

std::vector<DotPtr> Dots::GetDots() const
{
	// TODO: Group should be Group<DotPtr> so we don't have to do this...
	std::vector<DotPtr> rv;
	std::vector<PatternPtr> patterns = Group::GetPatterns();
	for (unsigned int i=0; i<patterns.size(); ++i)
	{
		DotPtr dot = boost::dynamic_pointer_cast<Dot>(patterns[i]);
		rv.push_back(dot);
	}
	return rv;
}


void Dots::SetOscillator(OscillatorPtr vib)
{
	LATERO_GRAPHICS_GUARD; 
	if (vib_) RemoveModifiableChild(vib_);
	std::vector<DotPtr> dotSet = GetDots(); 
	for (unsigned int i=0; i<dotSet.size(); ++i)
		dotSet[i]->SetOscillator(vib);
	vib_ = vib;
	AddModifiableChild(vib);
	SetLastModified_();
}


// Group's implementation is not efficient enough!
Cairo::RefPtr<Cairo::Pattern> Dots::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	std::vector<DotPtr> dots = GetDots();

	mmContext->push_group();
	if (dots.size() <= 0) return mmContext->pop_group();

	double radius = GetDotRadius();
	double height = GetHeight();
	mmContext->set_source_rgba(0,0,0,height);

	for (unsigned int i=0; i<dots.size(); ++i)
	{
		Point pos = dots[i]->GetPos();

		mmContext->save();
		mmContext->translate(pos.x,pos.y);
		mmContext->scale(radius,radius);
		mmContext->arc(0.0, 0.0, 1.0, 0, 2*M_PI);
		mmContext->restore();
		mmContext->fill();
	}
	return mmContext->pop_group();
}


} // namespace graphics
} // namespace latero



