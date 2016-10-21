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

#include "lineargratingtexture.h"
#include "lineargratingtexturewidget.h"
#include "../../xml.h"
#include "../../grating.h"
#include "../../units.h"
#include "stock.h"

namespace latero {
namespace graphics { 

LinearGratingTexture::LinearGratingTexture(const latero::Tactograph *dev, float pitch, float angle) :
	Pattern(dev), GratingTexture(dev,Point(0,0),"mm")
{
	SetAngle(angle);
	grating_->SetRidgeSize(pitch);
	grating_->SetVel(default_vel);
}

LinearGratingTexture::LinearGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), GratingTexture(dev,Point(0,0),"mm")
{
	SetAngle(default_angle);
	grating_->SetRidgeSize(default_pitch);
	grating_->SetVel(default_vel);

	LoadXML(node);
}


double LinearGratingTexture::NormRender_(const ActuatorState &state)
{
	Point rel = state.pos - seed_;
	float rad = angle_*M_PI/180;
	float d = rel.x*cos(rad) + rel.y*sin(rad);
	return grating_->Render_(d,state.GetTimeElapsed());
}



Cairo::RefPtr<Cairo::Pattern> LinearGratingTexture::GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();

	double angle = GetAngle();
	Point seed = GetSeed();
	GratingPtr grating = grating_; // TODO: clone since used unlocked
	double delta = grating->GetMinRidgeSize()/10;

	mmContext->translate(-seed.x, -seed.y);
	mmContext->rotate_degrees(angle);

	double xmin, xmax, dummy1, dummy2;
	mmContext->get_clip_extents(xmin, dummy1, xmax, dummy2);
	double w = xmax-xmin;

	Cairo::RefPtr<Cairo::LinearGradient> gradient = Cairo::LinearGradient::create(xmin,0,xmax,0);
	for (float x=xmin; x<xmax; x+=delta)
		gradient->add_color_stop_rgba((x-xmin)/w,0,0,0,grating->Render_(x,t));
	
	mmContext->set_source(gradient);
	mmContext->paint();

	mmContext->rotate_degrees(-angle);
	mmContext->translate(seed.x, seed.y);

	return mmContext->pop_group();
}

Gtk::Widget *LinearGratingTexture::CreateWidget(TexturePtr ptr)
{
	LinearGratingTexturePtr gen = boost::dynamic_pointer_cast<LinearGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new LinearGratingTextureWidget(gen);
}

Gtk::Widget *LinearGratingTexture::CreateAdvancedWidget(TexturePtr ptr)
{
	LinearGratingTexturePtr gen = boost::dynamic_pointer_cast<LinearGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new LinearGratingTextureAdvancedWidget(gen);
}

void LinearGratingTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("LinearGrating");
	node.AddChild("angle",GetAngle(),units::degree);
	GratingTexture::AppendXML(node);
}

void LinearGratingTexture::LoadXML(const XMLInputNode &node)
{
	XMLInputNode angleNode = node.GetChild("angle");
	if (angleNode) SetAngle(angleNode.GetFloat());

	GratingTexture::LoadXML(node);
}

std::string LinearGratingTexture::GetDefaultIconFile() const
{
	return tx_icon_linear_grating;
}

void LinearGratingTexture::SetAngle(float v)
{
	LATERO_GRAPHICS_GUARD;
	SetLastModified_();
	angle_ = v; // TODO: does this work with negative angles???
}

} // namespace graphics
} // namespace latero
