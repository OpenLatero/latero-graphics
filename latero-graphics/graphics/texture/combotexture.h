#pragma once

#include <memory>
#include "texture.h"
#include "../../graphics/group.h"

namespace latero::graphics {

typedef std::shared_ptr<class ComboTexture> ComboTexturePtr;

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
	virtual std::string GetName() { return "Composite Texture"; } // TODO: make this name match the class name?

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

} // namespace

