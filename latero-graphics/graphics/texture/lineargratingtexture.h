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

#ifndef LATERO_GRAPHICS_PLANAR_LINEAR_GRATING_TEXTURE
#define LATERO_GRAPHICS_PLANAR_LINEAR_GRATING_TEXTURE

#include "gratingtexture.h"
#include "lineargratingtexturefwd.h"

namespace latero {
namespace graphics { 

class LinearGratingTexture : public GratingTexture
{
public:
	static const int default_pitch = 8;
	static const int default_angle = 0;
	static const int default_vel = 0;

	static LinearGratingTexturePtr Create(const latero::Tactograph *dev, float pitch=default_pitch, float angle=default_angle)
	{
		return LinearGratingTexturePtr(new LinearGratingTexture(dev,pitch,angle));
	}

	static LinearGratingTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return LinearGratingTexturePtr(new LinearGratingTexture(dev,node));
	}

	virtual ~LinearGratingTexture() {}

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual double NormRender_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

	void SetAngle(float v);
	float GetAngle() const { LATERO_GRAPHICS_GUARD; return angle_; }

protected:
	LinearGratingTexture(const latero::Tactograph *dev, float pitch, float angle);
	LinearGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node);

	float angle_; // orientation in degrees
};

} // namespace graphics
} // namespace latero

#endif

