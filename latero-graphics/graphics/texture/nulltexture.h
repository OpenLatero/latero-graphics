#pragma once

#include <memory>
#include "texture.h"

namespace latero::graphics {

typedef std::shared_ptr<class NullTexture> NullTexturePtr;

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

} // namespace

