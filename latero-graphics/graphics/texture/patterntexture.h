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

#ifndef LATERO_GRAPHICS_PLANAR_PATTERN_TEXTURE
#define LATERO_GRAPHICS_PLANAR_PATTERN_TEXTURE

#include "texture.h"
#include "patterntexturefwd.h"

namespace latero {
namespace graphics { 

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

} // namespace graphics
} // namespace latero

#endif

