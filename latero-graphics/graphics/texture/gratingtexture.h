#pragma once

#include <memory>
#include "texture.h"
#include "../../grating.h"

namespace latero::graphics {

typedef std::shared_ptr<class GratingTexture> GratingTexturePtr;

class GratingTexture : public Texture
{
public:
	GratingTexture(const latero::Tactograph *dev, const Point &seed, std::string units);
	virtual ~GratingTexture() {};

	void SetSeed(const Point &p) 	{ LATERO_GRAPHICS_GUARD; SetLastModified_(); seed_ = p; }
	Point GetSeed() const 		{ LATERO_GRAPHICS_GUARD; return seed_; }

	GratingPtr GetGrating() const 	{ LATERO_GRAPHICS_GUARD; return grating_; }

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual Cairo::RefPtr<Cairo::Pattern> GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);
	virtual Cairo::RefPtr<Cairo::Pattern> GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t) = 0;
protected:

	Point seed_; 		// starting point of the grating
	GratingPtr grating_; 	// waveform
};

} // namespace

