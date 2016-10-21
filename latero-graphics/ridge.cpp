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

#include "ridge.h"
#include "units.h"

namespace latero {
namespace graphics { 

Ridge::Ridge(double edgeWidth) :
    width_(width_default),
	edgeWidth_(edgeWidth),
	txEnable_(txEnable_default),
	txAmp_(txAmp_default),
	txNbCycles_(txNbCycles_default)
{
}

void Ridge::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("ridge");
	node.AddChild("width", GetWidth(), units::mm);
	node.AddChild("edge_width", GetEdgeWidth(), units::mm);

	XMLOutputNode txnode = node.AddChild("texture");
	txnode.AddChild("enable", GetTxEnable());
	txnode.AddChild("amplitude", GetTxAmp(), units::percent);
	txnode.AddChild("nb_cycles", GetTxNbCycles());
}

void Ridge::LoadXML(const XMLInputNode &root)
{
	XMLInputNode ridgenode = root.GetChild("ridge");
	if (ridgenode)
	{
		XMLInputNode node = ridgenode.GetChild("width");
		if (node) SetWidth(node.GetFloat());
;
		node = ridgenode.GetChild("edge_width");
		if (node) SetEdgeWidth(node.GetFloat());

		XMLInputNode txnode = ridgenode.GetChild("texture");
		if (txnode)
		{
			node = txnode.GetChild("enable");
			if (node) SetTxEnable(node.GetBool());

			node = txnode.GetChild("amplitude");
			if (node) SetTxAmp(node.GetFloat());

			node = txnode.GetChild("nb_cycles");
			if (node) SetTxNbCycles(node.GetInt());
		}
	}
}


float Ridge::GetValue_(float d)
{
	double effDist = fabs(d) / (width_/2);
	float rv = GetShape_(d);
	if (txEnable_ && txAmp_>0)
	{
		float tx = 0.5*txAmp_ * cos(effDist * 2 * M_PI * (txNbCycles_ + 0.5));
		rv = tx + (1.0-txAmp_)*rv + 0.5*txAmp_;
	}
	return rv;
}


double Ridge::GetShape_(double d, double width, double edge)
{
	double r = width/2;
	d = fabs(d);
	edge = fmin(edge,r);

	if (d < r - edge)	return 1.0;
	else if (d > r)		return 0.0;

	double v = (d - (r-edge)) / edge;
	return 0.5 + 0.5*cos(v * M_PI);
}

} // namespace graphics
} // namespace latero
