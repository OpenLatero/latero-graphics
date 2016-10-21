// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#ifndef LATERO_GRAPHICS_CLOSED_PATTERN_H
#define LATERO_GRAPHICS_CLOSED_PATTERN_H

#include "closedpatternfwd.h"
#include "pattern.h"
#include "texture/texturefwd.h"

namespace latero {
namespace graphics { 

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

} // namespace graphics
} // namespace latero

#endif
