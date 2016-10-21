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

#ifndef LATERO_GRAPHICS_PLANAR_DOUBLE_LINEAR_GRATING_TEXTURE
#define LATERO_GRAPHICS_PLANAR_DOUBLE_LINEAR_GRATING_TEXTURE

#include "doublelineargratingtexturefwd.h"
#include "combotexture.h"
#include "lineargratingtexturefwd.h"
 
namespace latero {
namespace graphics { 

/** TODO: integrate velocity, support radial mode */

class DoubleLinearGratingTexture : public ComboTexture
{
public:
	enum { constraint_ortho, constraint_parallel };
	static const int constraint_default = constraint_ortho;

	static DoubleLinearGratingTexturePtr Create(const latero::Tactograph *dev)
	{
		return DoubleLinearGratingTexturePtr(new DoubleLinearGratingTexture(dev));
	}

	static DoubleLinearGratingTexturePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return DoubleLinearGratingTexturePtr(new DoubleLinearGratingTexture(dev,node));
	}

	virtual ~DoubleLinearGratingTexture() {}
	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual std::string GetDefaultIconFile() const;

	virtual Gtk::Widget *CreateWidget(TexturePtr ptr);
	virtual Gtk::Widget *CreateAdvancedWidget(TexturePtr ptr);

	void SetAngle(double v);
	double GetAngle() const;

	void SetConstraint(int mode);
	int GetConstraint() const;

	void SetSeed(const Point &p);
	Point GetSeed() const;

	void SetPrimaryRidgeSize(double v);
	double GetPrimaryRidgeSize() const;

	void SetPrimaryGapSize(double v);
	double GetPrimaryGapSize() const;

	void SetSecondaryScale(double v);
	double GetSecondaryScale() const;

	void SetPrimaryAmplitude(double v);
	double GetPrimaryAmplitude() const;

	void SetSecondaryAmplitude(double v);
	double GetSecondaryAmplitude() const;

	virtual void OnMotionChange() { UpdateChildMotion(); };
	void UpdateChildMotion();

	// should not be used...
	LinearGratingTexturePtr GetPrimaryGrating() { return tx_[0]; }
	LinearGratingTexturePtr GetSecondaryGrating() { return tx_[1]; }

	virtual bool ChildrenArePublic() { return false; }

protected:
	DoubleLinearGratingTexture(const latero::Tactograph *dev);
	DoubleLinearGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node);

 	int constraint_;
	double secScale_;
	LinearGratingTexturePtr tx_[2];
};

} // namespace graphics
} // namespace latero

#endif

