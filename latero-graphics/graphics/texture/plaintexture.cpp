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

#include "plaintexture.h"
#include "plaintexturewidget.h"
#include "texture.h"
#include "../../xml.h"
#include "../../units.h"
#include "stock.h"

namespace latero {
namespace graphics { 

PlainTexture::PlainTexture(const latero::Tactograph *dev) : 
	Pattern(dev), Texture(dev)
{
}

PlainTexture::PlainTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), Texture(dev)
{
	LoadXML(node);
}

Cairo::RefPtr<Cairo::Pattern> PlainTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();
	mmContext->set_source_rgba(0,0,0,1);
	mmContext->paint();
	return mmContext->pop_group();
}

Gtk::Widget *PlainTexture::CreateWidget(TexturePtr ptr)
{
	PlainTexturePtr gen = boost::dynamic_pointer_cast<PlainTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new PlainTextureWidget(gen);
}

void PlainTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Plain");
	Texture::AppendXML(node);
}

void PlainTexture::LoadXML(const XMLInputNode &node)
{
	Texture::LoadXML(node);
}

std::string PlainTexture::GetDefaultIconFile() const
{
	return tx_icon_null;
}


} // namespace graphics
} // namespace latero
