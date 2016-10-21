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

#ifndef LATERO_GRAPHICS_PLANAR_GRATING_TEXTURE
#define LATERO_GRAPHICS_PLANAR_GRATING_TEXTURE

#include "gratingtexturefwd.h"
#include "texture.h"
#include "../../gratingfwd.h"

namespace latero {
namespace graphics { 

class GratingTexture : public Texture
{
public:
	GratingTexture(const latero::Tactograph *dev, const Point &seed, std::string units);
	virtual ~GratingTexture() {};

	void SetSeed(const Point &p) 	{ LATERO_GRAPHICS_GUARD; SetLastModified_(); seed_ = p; }
	Point GetSeed() const 		{ LATERO_GRAPHICS_GUARD; return seed_; }

	GratingPtr GetGrating() const 	{ LATERO_GRAPHICS_GUARD; return grating_; }

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t) = 0;
protected:

	Point seed_; 		// starting point of the grating
	GratingPtr grating_; 	// waveform
};

} // namespace graphics
} // namespace latero

#endif

