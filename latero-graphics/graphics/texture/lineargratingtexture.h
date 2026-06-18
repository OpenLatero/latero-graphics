#pragma once

#include <memory>
#include "gratingtexture.h"

namespace latero::graphics {

typedef std::shared_ptr<class LinearGratingTexture> LinearGratingTexturePtr;

class LinearGratingTexture : public GratingTexture
{
public:
	static const int default_pitch = 8;
	static const int default_angle = 0;
	static const int default_vel = 0;

	static LinearGratingTexturePtr Create(const latero::Tactograph *dev, float pitch=default_pitch, float angle=default_angle)
	{
		return LinearGratingTexturePtr(new LinearGratingTexture(dev,pitch,angle));
	}

	static LinearGratingTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return LinearGratingTexturePtr(new LinearGratingTexture(dev,node));
	}

	virtual ~LinearGratingTexture() {}

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual double NormRender_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

	void SetAngle(float v);
	float GetAngle() const { LATERO_GRAPHICS_GUARD; return angle_; }

protected:
	LinearGratingTexture(const latero::Tactograph *dev, float pitch, float angle);
	LinearGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node);

	float angle_; // orientation in degrees
};

} // namespace

