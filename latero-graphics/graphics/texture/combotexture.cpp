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

#include "combotexture.h"
#include "../../graphics/groupwidget.h"
#include "texture.h"
#include "../../xml.h"
#include "../../units.h"
#include "patternops.h"
#include "stock.h"

namespace latero {
namespace graphics { 

ComboTexture::ComboTexture(const latero::Tactograph *dev) : 
	Pattern(dev), Texture(dev), Group(dev)
{
}

ComboTexture::ComboTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), Texture(dev), Group(dev)
{
	LoadXML(node);
}


double ComboTexture::NormRender_(const ActuatorState &state)
{
	return Group::DoRender_(state); // todo: should this be "Render" instead?
}

double ComboTexture::NormRenderShadow_(const ActuatorState &state)
{
	return Group::DoRenderShadow_(state);
}


Cairo::RefPtr<Cairo::Pattern> ComboTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	return Group::GetDrawingPattern(mmContext,t);
}


Gtk::Widget *ComboTexture::CreateWidget(TexturePtr ptr)
{
	ComboTexturePtr gen = boost::dynamic_pointer_cast<ComboTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new GroupWidget(gen);
}


void ComboTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Combo");
	
	Texture::AppendXML(node);
	Group::AppendOperation(node);
	Group::AppendPatterns(node);
}


void ComboTexture::LoadXML(const XMLInputNode &root)
{
	Group::LoadOperation(root);
	Group::LoadPatterns(root);
	Texture::LoadXML(root);
}



std::string ComboTexture::GetDefaultIconFile() const
{
	return tx_icon_combo;
}




std::vector<TexturePtr> ComboTexture::GetTextures() const
{
	std::vector<TexturePtr>	rv;
	std::vector<PatternPtr> patterns = GetPatterns();
	for (unsigned int i=0; i<patterns.size(); ++i)
	{
		TexturePtr tx = boost::dynamic_pointer_cast<Texture>(patterns[i]);
		if (tx) rv.push_back(tx);
	}
	return rv;
}

} // namespace graphics
} // namespace latero

