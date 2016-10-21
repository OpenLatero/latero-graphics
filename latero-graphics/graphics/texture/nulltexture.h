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

#ifndef LATERO_GRAPHICS_PLANAR_NULL_TEXTURE
#define LATERO_GRAPHICS_PLANAR_NULL_TEXTURE

#include "texture.h"
#include "nulltexturefwd.h"

namespace latero {
namespace graphics { 

class NullTexture : public Texture
{
public:
	static NullTexturePtr Create(const latero::Tactograph *dev)
	{
		return NullTexturePtr(new NullTexture(dev));
	}

	static NullTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return NullTexturePtr(new NullTexture(dev));
	}

	virtual ~NullTexture() {}
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual double NormRender_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);

protected:
	NullTexture(const latero::Tactograph *dev);
};

} // namespace graphics
} // namespace latero

#endif

