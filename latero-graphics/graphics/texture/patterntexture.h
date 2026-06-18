#pragma once

#include <memory>
#include "texture.h"

namespace latero::graphics {

typedef std::shared_ptr<class PatternTexture> PatternTexturePtr;

class PatternTexture : public Texture
{
public:
	static PatternTexturePtr Create(const latero::Tactograph *dev)
	{
		return PatternTexturePtr(new PatternTexture(dev));
	}

	static PatternTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return PatternTexturePtr(new PatternTexture(dev,node));
	}

	virtual ~PatternTexture() {}

	virtual double NormRender_(const ActuatorState &state);
	virtual double NormRenderShadow_(const ActuatorState &state) { return 1; };

	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual std::string GetDefaultIconFile() const;

	void SetGridWidth(double v) { LATERO_GRAPHICS_GUARD; gridWidth_=v; SetLastModified_(); }
	double GetGridWidth() const { LATERO_GRAPHICS_GUARD; return gridWidth_; }

	void SetGridHeight(double v) { LATERO_GRAPHICS_GUARD; gridHeight_=v; SetLastModified_(); }
	double GetGridHeight() const { LATERO_GRAPHICS_GUARD; return gridHeight_; }

	void SetPattern(PatternPtr pattern);
	PatternPtr GetPattern() const { LATERO_GRAPHICS_GUARD; return pattern_; }

protected:

	PatternTexture(const latero::Tactograph *dev);
	PatternTexture(const latero::Tactograph *dev, const XMLInputNode &node);

	PatternPtr pattern_;
	double gridWidth_, gridHeight_;
};

} // namespace

