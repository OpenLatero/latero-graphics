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

#ifndef LATERO_GRAPHICS_PLANAR_COMBO_TEXTURE
#define LATERO_GRAPHICS_PLANAR_COMBO_TEXTURE

#include "combotexturefwd.h"
#include "texture.h"
#include "../../graphics/group.h"

namespace latero {
namespace graphics { 

class ComboTexture : public Texture, public Group
{
public:
	static ComboTexturePtr Create(const latero::Tactograph *dev)
	{
		return ComboTexturePtr(new ComboTexture(dev));
	}

	static ComboTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return ComboTexturePtr(new ComboTexture(dev,node));
	}

	virtual ~ComboTexture() {}

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual std::string GetDefaultIconFile() const;

	virtual double DoRender_(const ActuatorState &state) { return Texture::DoRender_(state); }
	virtual double DoRenderShadow_(const ActuatorState &state) { return Texture::DoRenderShadow_(state); }
	virtual std::string GetName() { return "Composite Texture"; }

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
	{ return Texture::GetDrawingPattern(mmContext, t); }

	virtual double NormRender_(const ActuatorState &state);
	virtual double NormRenderShadow_(const ActuatorState &state);
	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateWidget(PatternPtr ptr)
	{ return Texture::CreateWidget(ptr); }

	std::vector<TexturePtr> GetTextures() const;

protected:
	ComboTexture(const latero::Tactograph *dev);
	ComboTexture(const latero::Tactograph *dev, const XMLInputNode &node);

	//std::vector<TexturePtr> textures_;
};

} // namespace graphics
} // namespace latero

#endif

