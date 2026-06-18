#pragma once

#include <memory>
#include "texture.h"

namespace latero::graphics {

typedef std::shared_ptr<class PlainTexture> PlainTexturePtr;

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

} // namespace

