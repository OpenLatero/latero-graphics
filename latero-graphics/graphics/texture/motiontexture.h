#pragma once

#include <memory>
#include "combotexture.h"
#include "vibrotexture.h"
#include "plaintexture.h"
#include "../../grating.h"
#include "lineargratingtexture.h"

namespace latero::graphics {

typedef std::shared_ptr<class MotionTexture> MotionTexturePtr;

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

    constexpr static const double direction_default = 0;
    constexpr static const double velocity_default = 50;

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

} // namespace

