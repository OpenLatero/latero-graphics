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

#include "oscillator.h"
#include "units.h"
#include "graphics/texture/patternops.h"

namespace latero {
namespace graphics { 

const Oscillator::BlendMode Oscillator::blend_mode_high(0,"high");
const Oscillator::BlendMode Oscillator::blend_mode_low(1,"low");

Oscillator::Oscillator(double freq, double amplitude, bool enable) : 
	mod_(false), amplitude_(amplitude), enable_(enable), blendMode_(blend_mode_high)
{
	blendModes_.Append(blend_mode_high);
	blendModes_.Append(blend_mode_low);
	SetFreq(freq);
}


void Oscillator::AppendXML(XMLOutputNode &root)  const
{
	XMLOutputNode node = root.AddChild("vibration");
	node.AddChild("enable", GetEnable());
	node.AddChild("frequency", GetFreq(), units::hz);
	node.AddChild("amplitude", GetAmplitude());
	node.AddChild("blend_mode", GetBlendMode().label);
}

void Oscillator::LoadXML(const XMLInputNode &root)
{
	SetEnable(false);
	XMLInputNode node = root.GetChild("vibration");
	if (node)
	{
		SetEnable(true);

		XMLInputNode n = node.GetChild("amplitude");
		if (n) SetAmplitude(n.GetFloat());

		n = node.GetChild("frequency");
		if (n) SetFreq(n.GetFloat());

		n = node.GetChild("enable");
		if (n) SetEnable(n.GetBool());

		n = node.GetChild("blend_mode");
		if (n) SetBlendMode(node.GetString());
	}
}



Cairo::RefPtr<Cairo::Pattern> Oscillator::GetPattern(Cairo::RefPtr<Cairo::Context> mmContext, double k)
{
	// Note:
	// K is used only internally to simplify the operation when vibration is added to a texture. The vibration V is then
	// rendered as (k-1) + kV.

	// TODO: this works but is pretty slow... is there a sufficient advantage to using Patterns?
	//
	// Note:
	// - Loading the noise pattern from a PNG is slightly faster (1.9 vs 2.2 seconds)
	// - This is faster than using even 1-mm rectangles.
	// - Precomputing 1000 or even 10000 random value saves about 15% computation but results in artifacts

	// TODO: Doesn't seem to work on Ubuntu 10.10. Was it working in 10.04?

	mmContext->push_group();
	double clip_x1, clip_y1, clip_x2, clip_y2;
	mmContext->get_clip_extents(clip_x1, clip_y1, clip_x2, clip_y2);

	double clip_pix_x1 = clip_x1;
	double clip_pix_y1 = clip_y1;
	double clip_pix_x2 = clip_x2;
	double clip_pix_y2 = clip_y2;
	mmContext->user_to_device(clip_pix_x1, clip_pix_y1); // get number of pixels
	mmContext->user_to_device(clip_pix_x2, clip_pix_y2);
	int pix_sx = clip_pix_x2 - clip_pix_x1;
	int pix_sy = clip_pix_y2 - clip_pix_y1;

	Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,pix_sx,pix_sy);
	unsigned char *data = surface->get_data();
    
    for (int y=0; y<pix_sy; ++y)
    {
        unsigned char* row = data + y*surface->get_stride();
        for (int x=0; x<pix_sx; ++x)
        {
            unsigned char* pix = row + x*4;
            pix[0] = pix[1] = pix[2] = 0;
            pix[3] = (unsigned char)((1-k)*255 + k*(rand()%256));
        }
    }
    surface->mark_dirty();

	double pix_per_mm_x = 1.0;
	double pix_per_mm_y = 1.0;
	mmContext->user_to_device_distance(pix_per_mm_x, pix_per_mm_y);
	if ((clip_x2>0)&&(clip_y2>0))
	{
		mmContext->scale(1/pix_per_mm_x, 1/pix_per_mm_y);
		mmContext->set_source(surface, clip_pix_x1, clip_pix_y1);
		mmContext->paint();
		mmContext->scale(pix_per_mm_x, pix_per_mm_y);
	}

	//mmContext->set_source_rgba(0,0,0,1);
	//mmContext->paint();
	return mmContext->pop_group();
}

void Oscillator::SetBlendMode(std::string label)
{
	BlendMode *op = blendModes_.Get(label);
	if (op) SetBlendMode(*op);
}

Cairo::RefPtr<Cairo::Pattern> Oscillator::GetBlendPattern(Cairo::RefPtr<Cairo::Pattern> base, Cairo::RefPtr<Cairo::Context> mmContext)
{
	bool invert = (blendMode_ == blend_mode_low);
	Cairo::RefPtr<Cairo::Pattern> rv = base;
	if (invert) rv = PatternOps::Invert(mmContext,rv);
	rv = PatternOps::Multiply(mmContext,rv,GetBlendPattern(mmContext));
	if (invert) rv = PatternOps::Invert(mmContext,rv);
	return rv;
}

} // namespace graphics
} // namespace latero


