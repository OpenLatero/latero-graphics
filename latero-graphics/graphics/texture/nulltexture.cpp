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

#include "nulltexture.h"
#include "texture.h"
#include "stock.h"

namespace latero {
namespace graphics { 

NullTexture::NullTexture(const latero::Tactograph *dev) :
	Pattern(dev), Texture(dev)
{
}

double NullTexture::NormRender_(const ActuatorState &state)
{
	return 0.0;
}

Cairo::RefPtr<Cairo::Pattern> NullTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();
	mmContext->set_source_rgba(0,0,0,0);
	mmContext->paint();
	return mmContext->pop_group();
}

Gtk::Widget *NullTexture::CreateWidget(TexturePtr ptr)
{
	return Texture::CreateWidget(ptr); // default
}

void NullTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Null");
	Texture::AppendXML(node);
}

std::string NullTexture::GetDefaultIconFile() const
{
	return tx_icon_null;
}

} // namespace graphics
} // namespace latero
