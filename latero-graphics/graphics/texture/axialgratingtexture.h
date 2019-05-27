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

#ifndef LATERO_GRAPHICS_PLANAR_AXIAL_GRATING_TEXTURE
#define LATERO_GRAPHICS_PLANAR_AXIAL_GRATING_TEXTURE

#include "axialgratingtexturefwd.h"
#include "gratingtexture.h"

namespace latero {
namespace graphics { 

class AxialGratingTexture : public GratingTexture
{
public:
    constexpr static const double default_ridgeSize = 36;
    constexpr static const double default_vel = 0;
    constexpr static const double default_seedAngle = 0;

	// The grating will gradually be saturated between the following
	// arc lenghts to prevent the spatial frequency to increase too
	// high near the center. 
    constexpr static const double min_arc_length2 = 1.0;
    constexpr static const double min_arc_length1 = 0.5;

	static AxialGratingTexturePtr Create(const latero::Tactograph *dev, Point seed = Point(0,0), uint axialNb=360/default_ridgeSize)
	{
		return AxialGratingTexturePtr(new AxialGratingTexture(dev,seed,axialNb));
	}

	static AxialGratingTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return AxialGratingTexturePtr(new AxialGratingTexture(dev,node));
	}

	virtual ~AxialGratingTexture() {}

	virtual std::string GetDefaultIconFile() const;
	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual double NormRender_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

	void SetSeedAngle(double v) 	{ LATERO_GRAPHICS_GUARD; SetLastModified_(); seedAngle_ = v; }
	double GetSeedAngle() const 	{ LATERO_GRAPHICS_GUARD; return seedAngle_; }

protected:
	AxialGratingTexture(const latero::Tactograph *dev, Point seed, uint axialNb);
	AxialGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node);

	// currently needed to allow modulation near 0 degree
	double seedAngle_;
};

} // namespace graphics
} // namespace latero

#endif

