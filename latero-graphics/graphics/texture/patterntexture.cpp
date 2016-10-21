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

#include "patterntexture.h"
#include "patterntexturewidget.h"
#include "texture.h"
#include "../../xml.h"
#include "../../units.h"
#include "stock.h"
#include "../../graphics/circle.h"
#include "../../graphics/stroke.h"

namespace latero {
namespace graphics { 

PatternTexture::PatternTexture(const latero::Tactograph *dev) : 
	Pattern(dev), Texture(dev), gridWidth_(20), gridHeight_(20)
{
	CirclePtr circle = Circle::Create(dev, Point(10,10), 5);
	circle->GetStroke()->SetWidth(1);
	SetPattern(circle);
}

PatternTexture::PatternTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), Texture(dev), gridWidth_(20), gridHeight_(20)
{
	CirclePtr circle = Circle::Create(dev, Point(10,10), 5);
	circle->GetStroke()->SetWidth(1);
	SetPattern(circle);

	LoadXML(node);
}

double PatternTexture::NormRender_(const ActuatorState &state)
{
	if (!pattern_) return 0;

	ActuatorState s = state;
	s.pos.x = fmod(s.pos.x, gridWidth_);
	if (s.pos.x<0) s.pos.x += gridWidth_;
	s.pos.y = fmod(s.pos.y, gridHeight_);
	if (s.pos.y<0) s.pos.y += gridHeight_;
	return pattern_->Render_(s);
};

Cairo::RefPtr<Cairo::Pattern> PatternTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	if (pattern_)
	{
		double gridWidth = GetGridWidth();
		double gridHeight = GetGridHeight();
		double w = Dev()->GetSurfaceWidth();
		double h = Dev()->GetSurfaceHeight();

		mmContext->push_group();
		for (double x=0; x<w; x+=gridWidth)
		for (double y=0; y<h; y+=gridHeight)
		{
			mmContext->save();
			mmContext->translate(x,y);
			mmContext->rectangle(0,0,gridWidth,gridHeight);
			mmContext->clip();
			mmContext->set_source(pattern_->GetDrawingPattern(mmContext,t));
			mmContext->paint();
			mmContext->restore();
		}
		return mmContext->pop_group();
	}
	else
	{
		mmContext->push_group();
		return mmContext->pop_group();
	}
}

Gtk::Widget *PatternTexture::CreateWidget(TexturePtr ptr)
{
	PatternTexturePtr gen = boost::dynamic_pointer_cast<PatternTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new PatternTextureWidget(gen);
}

Gtk::Widget *PatternTexture::CreateAdvancedWidget(TexturePtr ptr)
{
	PatternTexturePtr gen = boost::dynamic_pointer_cast<PatternTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new PatternTextureAdvancedWidget(gen);
}


void PatternTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Pattern");

	node.AddChild("grid_width", GetGridWidth(), units::mm);
	node.AddChild("grid_height", GetGridHeight(), units::mm);
	if (pattern_) pattern_->AppendXML(node);

	Texture::AppendXML(node);
}

void PatternTexture::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("grid_width");
	if (node) SetGridWidth(node.GetFloat());

	node = root.GetChild("grid_height");
	if (node) SetGridHeight(node.GetFloat());

	node = root.GetChild("Pattern");
	if (node) SetPattern(Pattern::Create(Dev(), node));

	Texture::LoadXML(node);
}

std::string PatternTexture::GetDefaultIconFile() const
{
	return tx_icon_pattern_grid;
}

void PatternTexture::SetPattern(PatternPtr pattern)
{
	LATERO_GRAPHICS_GUARD;
	if (pattern_) RemoveModifiableChild_(pattern_);
	pattern_ = pattern;
	AddModifiableChild_(pattern_);
}

} // namespace graphics
} // namespace latero
