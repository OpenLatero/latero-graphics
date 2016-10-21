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

#ifndef LATERO_GRAPHICS_PLANAR_BLEND_TEXTURE
#define LATERO_GRAPHICS_PLANAR_BLEND_TEXTURE

#include "texture.h"
#include "blendtexturefwd.h"
#include "../modulator/modulatorfwd.h"

namespace latero {
namespace graphics { 

// TODO: This could be implemented as part of ComboTexture using Group's reactive operator.
class BlendTexture : public Texture
{
public:
	static BlendTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return BlendTexturePtr(new BlendTexture(dev,node));
	}

	static BlendTexturePtr Create(const latero::Tactograph *dev)
		{ return BlendTexturePtr(new BlendTexture(dev)); }	

	virtual ~BlendTexture() {}

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual double NormRender_(const ActuatorState &state);
	virtual double GetAlpha_(const ActuatorState &state);
	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);

	TexturePtr GetFirstTexture() const { LATERO_GRAPHICS_GUARD; return tx_[0]; }
	TexturePtr GetSecondTexture() const { LATERO_GRAPHICS_GUARD; return tx_[1]; }

	void SetFirstTexture(TexturePtr tx) { SetTexture(0,tx); }
	void SetSecondTexture(TexturePtr tx) { SetTexture(1,tx); };
	void SetTexture(uint i, TexturePtr tx);

	void SetModulator(ModulatorPtr mod);
	ModulatorPtr GetModulator() const { LATERO_GRAPHICS_GUARD; return mod_; }

	typedef enum { ill_mode_mix, ill_mode_first, ill_mode_second } ill_mode;
	void SetIllMode(ill_mode mode) { LATERO_GRAPHICS_GUARD; illMode_ = mode; }
	ill_mode GetIllMode() { LATERO_GRAPHICS_GUARD; return illMode_; }

protected:
	BlendTexture(const latero::Tactograph *dev, const XMLInputNode &node);
	BlendTexture(const latero::Tactograph *dev);

	ModulatorPtr mod_;
	TexturePtr tx_[2];
	ill_mode illMode_;
};

} // namespace graphics
} // namespace latero

#endif

