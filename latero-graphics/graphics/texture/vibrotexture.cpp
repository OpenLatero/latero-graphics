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

#include "vibrotexture.h"
#include "vibrotexturewidget.h"
#include "texture.h"
#include "../../xml.h"
#include "../../units.h"
#include "stock.h"

namespace latero {
namespace graphics { 

VibroTexture::VibroTexture(const latero::Tactograph *dev, double freqHz) : 
	Pattern(dev), Texture(dev), vib_(Oscillator::Create(freqHz,1,true))
{
	SetFreq(freqHz);
}

VibroTexture::VibroTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), Texture(dev), vib_(Oscillator::Create(50,1,true))
{
	SetFreq(50);
	LoadXML(node);
}

Cairo::RefPtr<Cairo::Pattern> VibroTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	return vib_->GetPattern(mmContext);
}

Gtk::Widget *VibroTexture::CreateWidget(TexturePtr ptr)
{
	VibroTexturePtr gen = boost::dynamic_pointer_cast<VibroTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new VibroTextureWidget(gen);
}

void VibroTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Vibration");
	node.AddChild("freq", GetFreq(), units::hz);
	Texture::AppendXML(node);
}

void VibroTexture::LoadXML(const XMLInputNode &node)
{
	XMLInputNode freqNode = node.GetChild("freq");
	if (freqNode) SetFreq(freqNode.GetFloat());
	Texture::LoadXML(node);
}

std::string VibroTexture::GetDefaultIconFile() const
{
	return tx_icon_vibration;
}

} // namespace graphics
} // namespace latero
