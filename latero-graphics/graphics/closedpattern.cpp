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

#include "closedpattern.h"
#include "../units.h"
#include "texture/texture.h"
#include "texture/stock.h"

namespace latero {
namespace graphics { 

ClosedPattern::ClosedPattern(const latero::Tactograph *dev) : Pattern(dev)
{
	txGap_ = 0;
	txEdgeSize_ = 1;
	txEnable_ = false;
	texture_ = Texture::Create(Dev(), tx_grating_horizontal);
	AddModifiableChild(texture_);
}

void ClosedPattern::SetFillTexture(TexturePtr tx)
{
	LATERO_GRAPHICS_GUARD;
	ReplaceModifiableChild_(texture_,tx);
	texture_=tx;
	SetLastModified_();
}


void ClosedPattern::LoadXML(const XMLInputNode &root)
{
	XMLInputNode txNode = root.GetChild("fill_texture");
	if (txNode)
	{
		XMLInputNode node = txNode.GetChild("enable");
		if (node) SetFillTextureEnable(node.GetBool());

		node = txNode.GetChild("gap");
		if (node) SetFillTextureGap(node.GetFloat());

		node = txNode.GetChild("edge_size");
		if (node) SetFillTextureEdgeSize(node.GetFloat());

		node = txNode.GetChild("Pattern");
		if (node) SetFillTexture(Texture::Create(Dev(),node));
	}
}

void ClosedPattern::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode txNode = root.AddChild("fill_texture");
	txNode.AddChild("enable", GetFillTextureEnable());
	txNode.AddChild("gap", GetFillTextureGap(), units::mm);
	txNode.AddChild("edge_size", GetFillTextureEdgeSize(), units::mm);
	texture_->AppendXML(txNode);
}

double ClosedPattern::RenderFilling_(double dFromStroke, const ActuatorState &state)
{
	double gap = GetFillTextureGap_();
	double edge = GetFillTextureEdgeSize_();
	double d = dFromStroke;

	if (gap < 0)
	{
		// gap is negative: texture extends all the way to the centerline
		return GetFillTexture_()->Render_(state);
	}
	else
	{
		// gap is positive: first a gap, then texture amplitude increases linearly over a distance of "edge"
		double alpha = fmin(1, (d - gap) / edge);
		if (alpha <= 0)
			return 0;
		else
			return alpha * GetFillTexture_()->Render_(state);
	}
}

} // namespace graphics
} // namespace latero


