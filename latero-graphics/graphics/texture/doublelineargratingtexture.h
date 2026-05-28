#pragma once

#include "doublelineargratingtexturefwd.h"
#include "combotexture.h"
#include "lineargratingtexturefwd.h"
 
namespace latero::graphics {

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

} // namespace

