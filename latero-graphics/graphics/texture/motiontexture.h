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

#ifndef LATERO_GRAPHICS_PLANAR_MOTION_TEXTURE
#define LATERO_GRAPHICS_PLANAR_MOTION_TEXTURE

#include "motiontexturefwd.h"
#include "combotexture.h"
#include "vibrotexture.h"
#include "plaintexture.h"
#include "../../grating.h"
#include "lineargratingtexture.h"
 
namespace latero {
namespace graphics { 

class MotionTexture : public ComboTexture
{
public:
	typedef NamedID CueType;
	typedef NamedIDSet CueTypeSet;
	static const CueType 	cue_type_none, 
				cue_type_vib_bands, 
				cue_type_superposed_bands,
				cue_type_superposed_vib_bands,
				cue_type_disabling_bands,
				cue_type_blend_bands;

	static const double direction_default = 0;
	static const double velocity_default = 50;

	static MotionTexturePtr Create(const latero::Tactograph *dev)
	{
		return MotionTexturePtr(new MotionTexture(dev));
	}

	static MotionTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return MotionTexturePtr(new MotionTexture(dev,node));
	}

	virtual ~MotionTexture() {}
	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);

	TexturePtr GetTexture() const  { LATERO_GRAPHICS_GUARD; return tx_; }
	void SetTexture(TexturePtr tx);

	TexturePtr GetMotionTexture() const  { LATERO_GRAPHICS_GUARD; return motionTx_; }

	CueTypeSet GetCueTypes() const { return cueTypeSet_; };
	CueType GetCueType() const	{ LATERO_GRAPHICS_GUARD; return cueType_; }
	void SetCueType(const CueType &t);
	void SetCueType(std::string label);

	double GetDirection() const { LATERO_GRAPHICS_GUARD; return direction_; }
	void SetDirection(double v);

	// todo: could be in Hz?
	// use abstractions? e.g slow to fast...?
	double GetVelocity() const { LATERO_GRAPHICS_GUARD; return velocity_; }
	void SetVelocity(double v);

	virtual bool ChildrenArePublic() { return false; }	

protected:
	MotionTexture(const latero::Tactograph *dev);
	MotionTexture(const latero::Tactograph *dev, const XMLInputNode &node);
	void SetDefaults();
	
	CueType cueType_;
	CueTypeSet cueTypeSet_;

	double direction_; // angle between 0 and 360 degrees
	double velocity_; // in mm/s

	TexturePtr tx_;
	VibroTexturePtr vibTx_;
	LinearGratingTexturePtr motionTx_;
	PlainTexturePtr plainTx_;
};

} // namespace graphics
} // namespace latero

#endif

