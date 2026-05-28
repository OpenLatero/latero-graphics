#pragma once

#include "axialgratingtexturefwd.h"
#include "gratingtexture.h"

namespace latero::graphics {

class AxialGratingTexture : public GratingTexture
{
public:
    constexpr static const double default_ridgeSize = 36;
    constexpr static const double default_vel = 0;
    constexpr static const double default_seedAngle = 0;

	// The grating will gradually be saturated between the following
	// arc lenghts to prevent the spatial frequency to increase too
	// high near the center. 
    constexpr static const double min_arc_length2 = 1.0;
    constexpr static const double min_arc_length1 = 0.5;

	static AxialGratingTexturePtr Create(const latero::Tactograph *dev, Point seed = Point(0,0), uint axialNb=360/default_ridgeSize)
	{
		return AxialGratingTexturePtr(new AxialGratingTexture(dev,seed,axialNb));
	}

	static AxialGratingTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return AxialGratingTexturePtr(new AxialGratingTexture(dev,node));
	}

	virtual ~AxialGratingTexture() {}

	virtual std::string GetDefaultIconFile() const;
	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual double NormRender_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

	void SetSeedAngle(double v) 	{ LATERO_GRAPHICS_GUARD; SetLastModified_(); seedAngle_ = v; }
	double GetSeedAngle() const 	{ LATERO_GRAPHICS_GUARD; return seedAngle_; }

protected:
	AxialGratingTexture(const latero::Tactograph *dev, Point seed, uint axialNb);
	AxialGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node);

	// currently needed to allow modulation near 0 degree
	double seedAngle_;
};

} // namespace

