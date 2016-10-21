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

#ifndef LATERO_GRAPHICS_PLANAR_VIBRO_TEXTURE
#define LATERO_GRAPHICS_PLANAR_VIBRO_TEXTURE

#include "texture.h"
#include "vibrotexturefwd.h"

namespace latero {
namespace graphics { 

class VibroTexture : public Texture
{
public:
	static VibroTexturePtr Create(const latero::Tactograph *dev, double freqHz = 50.0)
	{
		return VibroTexturePtr(new VibroTexture(dev,freqHz));
	}

	static VibroTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return VibroTexturePtr(new VibroTexture(dev,node));
	}

	virtual ~VibroTexture() {}

	virtual double NormRender_(const ActuatorState &state) { return vib_->Render_(state); };
	virtual double NormRenderShadow_(const ActuatorState &state) { return 1; };

	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);

	void SetFreq(float hz) { vib_->SetFreq(hz); }
	float GetFreq() const { return vib_->GetFreq(); };

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual std::string GetDefaultIconFile() const;

protected:
	// todo add parameters
	VibroTexture(const latero::Tactograph *dev, double freqHz = 50.0);
	VibroTexture(const latero::Tactograph *dev, const XMLInputNode &node);
	
	OscillatorPtr vib_;
};

} // namespace graphics
} // namespace latero

#endif

