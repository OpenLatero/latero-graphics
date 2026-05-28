#pragma once

#include "texture.h"
#include "vibrotexturefwd.h"

namespace latero::graphics {

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

} // namespace

