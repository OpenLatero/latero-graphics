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

#ifndef LATERO_GRAPHICS_PLANAR_PLAIN_TEXTURE
#define LATERO_GRAPHICS_PLANAR_PLAIN_TEXTURE

#include "texture.h"
#include "plaintexturefwd.h"

namespace latero {
namespace graphics { 

class PlainTexture : public Texture
{
public:
	static PlainTexturePtr Create(const latero::Tactograph *dev)
	{
		return PlainTexturePtr(new PlainTexture(dev));
	}

	static PlainTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return PlainTexturePtr(new PlainTexture(dev,node));
	}

	virtual ~PlainTexture() {}

	virtual double NormRender_(const ActuatorState &state) { return 1.0; };
	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual std::string GetDefaultIconFile() const;

protected:
	PlainTexture(const latero::Tactograph *dev);
	PlainTexture(const latero::Tactograph *dev, const XMLInputNode &node);
};

} // namespace graphics
} // namespace latero

#endif

