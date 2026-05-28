#pragma once

#include "gratingtexture.h"
#include "radialgratingtexturefwd.h"

namespace latero::graphics {

class RadialGratingTexture : public GratingTexture
{
public:
    constexpr static const double default_pitch = 8;
    constexpr static const double default_vel = 0;

	static RadialGratingTexturePtr Create(const latero::Tactograph *dev, Point seed = Point(0,0), float pitch=default_pitch)
	{
		return RadialGratingTexturePtr(new RadialGratingTexture(dev,seed,pitch));
	}

	static RadialGratingTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return RadialGratingTexturePtr(new RadialGratingTexture(dev,node));
	}

	virtual ~RadialGratingTexture();

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual double NormRender_(const ActuatorState &state);
	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

protected:
	RadialGratingTexture(const latero::Tactograph *dev, Point seed, float pitch);
	RadialGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node);
};

} // namespace

