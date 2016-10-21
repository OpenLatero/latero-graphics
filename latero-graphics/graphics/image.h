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

#ifndef LATERO_GRAPHICS_PLANAR_IMAGE_H
#define LATERO_GRAPHICS_PLANAR_IMAGE_H

#include "texture/texturefwd.h"
#include "pattern.h"
#include "../maskfwd.h"
#include "imagefwd.h"
#include "../gtk/animation.h"

namespace latero {
namespace graphics { 


class Image : public Pattern
{
public:
	static ImagePtr Create(const latero::Tactograph *dev, TexturePtr texture = TexturePtr() )
	{
		return ImagePtr(new Image(dev,texture));
	}


	static ImagePtr Create(const latero::Tactograph *dev, Glib::RefPtr<Gdk::Pixbuf> mask, TexturePtr texture = TexturePtr() )
	{
		return ImagePtr(new Image(dev,mask,texture));
	}

	static ImagePtr Create(const latero::Tactograph *dev, std::string mask, TexturePtr texture = TexturePtr())
	{
		return ImagePtr(new Image(dev,mask,texture));
	}

	static ImagePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return ImagePtr(new Image(dev, node));
	}

	virtual ~Image();
	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);
	virtual std::string GetName() { return "Image"; }

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	inline MaskPtr GetMask() { return mask_; }

	void Enable(bool v);

	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

	TexturePtr GetTexture() { LATERO_GRAPHICS_GUARD; return texture_; }
	void SetTexture(TexturePtr texture);
	
	virtual void LoadXML(const XMLInputNode &root);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual bool UpdateVisible_(const State *state);

protected:
	latero::graphics::gtk::Animation GetVibroIllustration(uint w, uint h);
	void DrawIllustration(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	TexturePtr texture_;
	MaskPtr mask_;
	bool enable_; // todo: move somewhere else!

	Image(const latero::Tactograph *dev, TexturePtr texture = TexturePtr());
	Image(const latero::Tactograph *dev, Glib::RefPtr<Gdk::Pixbuf> mask, TexturePtr texture = TexturePtr());
	Image(const latero::Tactograph *dev, std::string mask, TexturePtr texture = TexturePtr());
	Image(const latero::Tactograph *dev, const XMLInputNode &node);
};

} // namespace graphics
} // namespace latero

#endif
