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

#include "blendtexture.h"
#include "blendtexturewidget.h"
#include "texture.h"
#include "stock.h"
#include "patternops.h"
#include "../../units.h"
#include "../modulator/dirmodulator.h"
#include <gtkmm.h>

namespace latero {
namespace graphics { 

BlendTexture::BlendTexture(const latero::Tactograph *dev, const XMLInputNode &node) :
	Pattern(dev), Texture(dev), illMode_(ill_mode_mix)
{
	SetModulator(DirModulator::Create(dev));
	SetTexture(0, Texture::Create(dev,tx_grating_vertical));
	SetTexture(1, Texture::Create(dev,tx_grating_horizontal));
	LoadXML(node);
}

BlendTexture::BlendTexture(const latero::Tactograph *dev) :
	Pattern(dev), Texture(dev), illMode_(ill_mode_mix)
{
	SetModulator(DirModulator::Create(dev));
	SetTexture(0, Texture::Create(dev,tx_grating_vertical));
	SetTexture(1, Texture::Create(dev,tx_grating_horizontal));
}

double BlendTexture::NormRender_(const ActuatorState &state)
{
	double k = GetAlpha_(state); // todo: wasteful: should not be recomputed everytime!?
	double a = (k>0) ? tx_[0]->Render_(state) : 0;
	double b = (k<1) ? tx_[1]->Render_(state) : 0;
	return k*a + (1-k)*b;
}

double BlendTexture::GetAlpha_(const ActuatorState &state)
{
	// TODO: should be done once for all 64 actuators!
	return mod_->GetValue_(state);
}

Cairo::RefPtr<Cairo::Pattern> BlendTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	ill_mode mode = GetIllMode();
	if (mode == ill_mode_first)
		return GetFirstTexture()->GetDrawingPattern(mmContext,t);
	else if  (mode == ill_mode_second)
		return GetSecondTexture()->GetDrawingPattern(mmContext,t);
	// else...

	mmContext->push_group();

	//  top-left half
	mmContext->push_group();
	mmContext->set_source_rgba(1,1,1,1);
	//mmContext->move_to(0,0);
	//mmContext->line_to(Dev()->GetSurfaceWidth(),0);
	//mmContext->line_to(0,Dev()->GetSurfaceHeight());
	mmContext->rectangle(0,0, Dev()->GetSurfaceWidth()/2, Dev()->GetSurfaceHeight());
	//mmContext->line_to(Dev()->GetSurfaceWidth(),0);
	//mmContext->line_to(0,Dev()->GetSurfaceHeight());
	//mmContext->close_path();
	mmContext->fill();
	Cairo::RefPtr<Cairo::Pattern> topleft = mmContext->pop_group();

	// bottom-right half
	mmContext->push_group();
	mmContext->set_source_rgba(1,1,1,1);
	//mmContext->move_to(Dev()->GetSurfaceWidth(),Dev()->GetSurfaceHeight());
	//mmContext->line_to(Dev()->GetSurfaceWidth(),0);
	//mmContext->line_to(0,Dev()->GetSurfaceHeight());
	//mmContext->close_path();
	mmContext->rectangle(Dev()->GetSurfaceWidth()/2,0, Dev()->GetSurfaceWidth()/2, Dev()->GetSurfaceHeight());
	mmContext->fill();
	Cairo::RefPtr<Cairo::Pattern> bottomright = mmContext->pop_group();

	Cairo::RefPtr<Cairo::Pattern> a = GetFirstTexture()->GetDrawingPattern(mmContext,t);
	Cairo::RefPtr<Cairo::Pattern> b = GetSecondTexture()->GetDrawingPattern(mmContext,t);

	mmContext->set_source(a);
	mmContext->mask(topleft);

	mmContext->set_source(b);
	mmContext->mask(bottomright);

	if (mod_)
	{

		int h = 1000;
		int w = h*Dev()->GetSurfaceWidth()/Dev()->GetSurfaceHeight();
		int plotSize = 0.6*h;

		Glib::RefPtr<Gdk::Pixbuf> velplot = mod_->GetIllustration(plotSize);
		mmContext->save();
		mmContext->scale(Dev()->GetSurfaceWidth()/w,Dev()->GetSurfaceHeight()/h);

		int innerw = 30; //14
		int outerw = 50; //22
		mmContext->rectangle(outerw/2,outerw/2, w/2 - outerw, h - outerw);
		mmContext->set_line_width(outerw);
		mmContext->set_source_rgba(0,0,0,1.0);
		mmContext->stroke_preserve();
		mmContext->set_line_width(innerw);
		mmContext->set_source_rgba(1,1,1,1.0);
		mmContext->stroke();

		mmContext->rectangle(w/2 + outerw/2, outerw/2, w/2 - outerw, h - outerw);
		mmContext->set_line_width(outerw);
		mmContext->set_source_rgba(0,0,0,1.0);
		mmContext->stroke_preserve();
		mmContext->set_line_width(innerw);
		mmContext->set_source_rgba(0,0,0,1.0);
		mmContext->stroke();

		int arcw = outerw - innerw;
		mmContext->arc(w/2, h/2, 0.5*plotSize + arcw/2, 0, 2*M_PI);
		mmContext->set_line_width(arcw);
		mmContext->set_source_rgba(0.5,0.5,0.5,1.0);
		mmContext->fill();

		Gdk::Cairo::set_source_pixbuf(mmContext,velplot,w/2 - plotSize/2, h/2 - plotSize/2);
		mmContext->paint();
		mmContext->restore();
	}

	return mmContext->pop_group();
}

Gtk::Widget *BlendTexture::CreateWidget(TexturePtr ptr)
{
	BlendTexturePtr gen = boost::dynamic_pointer_cast<BlendTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new BlendTextureWidget(gen);
}

void BlendTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Blend");
	Texture::AppendXML(node);
	mod_->AppendXML(node);
	tx_[0]->AppendXML(node);
	tx_[1]->AppendXML(node);
}

void BlendTexture::LoadXML(const XMLInputNode &node)
{
	Texture::LoadXML(node);
	XMLInputNode modNode = node.GetChild("Modulator");
	if (modNode) SetModulator(Modulator::Create(Dev(), modNode));

	std::vector<XMLInputNode> txNodes = node.GetChildren("Pattern");
	if (txNodes.size() == 2)
	{
		SetTexture(0, Texture::Create(Dev(),txNodes[0]));
		SetTexture(1, Texture::Create(Dev(),txNodes[1]));
	}
}

void BlendTexture::SetModulator(ModulatorPtr mod)
{
	LATERO_GRAPHICS_GUARD;
	if (mod_) RemoveModifiableChild(mod_);
	mod_ = mod;
	AddModifiableChild(mod_);
}

std::string BlendTexture::GetDefaultIconFile() const
{
	return tx_icon_blend;
}

void BlendTexture::SetTexture(uint i, TexturePtr tx)
{
	if (i>1) return;
	LATERO_GRAPHICS_GUARD;
	if (tx_[i]) RemoveModifiableChild_(tx_[i]);
	tx_[i] = tx;
	AddModifiableChild(tx);
}

} // namespace graphics
} // namespace latero
