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

#include "gratingtexture.h"
#include "../../grating.h"
#include "../../xml.h"
#include "../../units.h"

namespace latero {
namespace graphics { 

GratingTexture::GratingTexture(const latero::Tactograph *dev, const Point &seed, std::string units) :
	Pattern(dev), Texture(dev),
	seed_(seed),
	grating_(Grating::Create(units))
{
	AddModifiableChild(grating_);
}

void GratingTexture::AppendXML(XMLOutputNode &node) const
{
	node.AddChild("seed", GetSeed(), units::mm);
	grating_->AppendXML(node);
	Texture::AppendXML(node);
}

void GratingTexture::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("seed");
	if (node) SetSeed(node.GetPoint());
	grating_->LoadXML(root);
	Texture::LoadXML(root);
}

Cairo::RefPtr<Cairo::Pattern> GratingTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	return GetGratingDrawingPattern(mmContext,t);
}

} // namespace graphics
} // namespace latero


