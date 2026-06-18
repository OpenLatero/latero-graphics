#pragma once

#include <memory>
#include "texture/texture.h"
#include "pattern.h"
#include "../mask.h"
#include "../gtk/animation.h"

namespace latero::graphics {

typedef std::shared_ptr<class Image> ImagePtr;

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

} // namespace

