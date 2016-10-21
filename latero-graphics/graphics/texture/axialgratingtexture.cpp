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

#include "axialgratingtexture.h"
#include "axialgratingtexturewidget.h"
#include <math.h>
#include "../../grating.h"
#include "../../xml.h"
#include "../../units.h"
#include "stock.h"

namespace latero {
namespace graphics { 

AxialGratingTexture::AxialGratingTexture(const latero::Tactograph *dev, Point seed, uint axialNb) :
	Pattern(dev), GratingTexture(dev,seed,units::degree), seedAngle_(default_seedAngle)
{
	GetGrating()->SetRegularizable(true);
	GetGrating()->SetLength(360);
	GetGrating()->SetVel(default_vel);
	GetGrating()->SetRidgeSize(360/axialNb); // TODO!
}

AxialGratingTexture::AxialGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), GratingTexture(dev,Point(0,0),units::degree), seedAngle_(default_seedAngle)
{
	GetGrating()->SetRegularizable(true);
	GetGrating()->SetLength(360);
	GetGrating()->SetVel(default_vel);
	GetGrating()->SetRidgeSize(default_ridgeSize);
	LoadXML(node);
}


double AxialGratingTexture::NormRender_(const ActuatorState &state)
{
	Point p = state.pos - seed_;
	double r = p.Norm();

	// saturate gradually past a certain arc length (TODO: precompute?)
	double cycle = units::DegreeToRad(grating_->GetMinCycleSize_());
	double r1 = min_arc_length1/cycle;
	double r2 = min_arc_length2/cycle;
	double delta = r2-r1;
	double mod = 1.0;
	if (r < r1)
		mod = 0;
	else if (r < r2)
		mod = (1/delta) * (r - r1);

	float d = (180.0/M_PI)*atan2(p.y,p.x) - seedAngle_;
	while (d<0) d+=360;

	return ((1-mod)*1.0 + mod*grating_->Render_(d,state.GetTimeElapsed()));
}

Cairo::RefPtr<Cairo::Pattern> AxialGratingTexture::GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();
	Point seed = GetSeed();

	Point clip1, clip2;
	mmContext->get_clip_extents(clip1.x, clip1.y, clip2.x, clip2.y);

	// find the longest diagonal (extend of disk around seed that must be drawn)
	Point diag1 = seed-clip1;
	Point diag2 = clip2-seed;
	Point ldiag(fmax(diag1.x,diag2.x),fmax(diag1.y,diag2.y));
	double r = ldiag.Norm();

	/** @ TODO: this is pretty slow... */
	mmContext->set_line_width(0.0);
	mmContext->translate(seed.x, seed.y);

	double delta = units::DegreeToRad(GetGrating()->GetMinRidgeSize())/50;
	for (double theta=0; theta<2*M_PI; theta+=delta)
	{
		mmContext->set_source_rgba(0,0,0,grating_->Render_(units::RadToDegree(theta),t));
		mmContext->move_to(0, 0);
		mmContext->line_to(r*cos(theta), r*sin(theta));
		mmContext->line_to(r*cos(theta+delta), r*sin(theta+delta));
		mmContext->close_path();
		mmContext->fill();
	}

	// draw rough approximation of saturated middle
	double cycle = units::DegreeToRad(grating_->GetMinCycleSize_());
	double rmid = (min_arc_length1+min_arc_length2)/(2*cycle);
	mmContext->set_source_rgba(0,0,0,1);
	mmContext->arc(0,0,rmid,0,2*M_PI);
	mmContext->fill();
	
	mmContext->translate(-seed.x, -seed.y);
	return mmContext->pop_group();
}

Gtk::Widget *AxialGratingTexture::CreateWidget(TexturePtr ptr)
{
	AxialGratingTexturePtr gen = boost::dynamic_pointer_cast<AxialGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new AxialGratingTextureWidget(gen);
}

Gtk::Widget *AxialGratingTexture::CreateAdvancedWidget(TexturePtr ptr)
{
	AxialGratingTexturePtr gen = boost::dynamic_pointer_cast<AxialGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new AxialGratingTextureAdvancedWidget(gen);
}


void AxialGratingTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("AxialGrating");
	node.AddChild("seed_angle", GetSeedAngle(), units::degree);
	GratingTexture::AppendXML(node);
}

void AxialGratingTexture::LoadXML(const XMLInputNode &node)
{
	GratingTexture::LoadXML(node);
	XMLInputNode n = node.GetChild("seed_angle");
	if (n) SetSeedAngle(n.GetFloat());
}

std::string AxialGratingTexture::GetDefaultIconFile() const
{
	return tx_icon_axial_grating;
}

} // namespace graphics
} // namespace latero


