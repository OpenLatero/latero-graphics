#pragma once

#include "closedpatternfwd.h"
#include "pattern.h"
#include "texture/texturefwd.h"

namespace latero::graphics {

class ClosedPattern : public Pattern
{
public:
	ClosedPattern(const latero::Tactograph *dev);
	virtual ~ClosedPattern() {};

	virtual void AppendXML(XMLOutputNode &root) const;
	virtual void LoadXML(const XMLInputNode &root);

	inline void SetFillTextureEnable(bool v) { LATERO_GRAPHICS_GUARD; txEnable_=v; SetLastModified_(); }
	inline bool GetFillTextureEnable() const { LATERO_GRAPHICS_GUARD; return txEnable_; }
	inline bool GetFillTextureEnable_() const { return txEnable_; }

	void SetFillTexture(TexturePtr tx);
	inline TexturePtr GetFillTexture() { LATERO_GRAPHICS_GUARD; return texture_; }
	inline TexturePtr GetFillTexture_() { return texture_; }

	inline void SetFillTextureGap(double v) { LATERO_GRAPHICS_GUARD; txGap_=v; SetLastModified_(); }
	inline double GetFillTextureGap() const { LATERO_GRAPHICS_GUARD; return txGap_; }
	inline double GetFillTextureGap_() const { return txGap_; }

	inline void SetFillTextureEdgeSize(double v) { LATERO_GRAPHICS_GUARD; txEdgeSize_=v; SetLastModified_(); }
	inline double GetFillTextureEdgeSize() const { LATERO_GRAPHICS_GUARD; return txEdgeSize_; }
	inline double GetFillTextureEdgeSize_() const { return txEdgeSize_; }

protected:
	/**
	 * @returns filling texture value
	 * @param dFromStroke distance from the outer edge of the stroke
	 * @param state current system state
	 */
	double RenderFilling_(double dFromStroke, const ActuatorState &state);

private:
	
	bool txEnable_;
	double txGap_; // if negative, the texture extends to the center line
	double txEdgeSize_;
	TexturePtr texture_;
};

} // namespace

