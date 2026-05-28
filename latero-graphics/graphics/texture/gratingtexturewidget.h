#pragma once

#include <gtkmm.h>
#include "texturewidget.h"
#include "../patternpreview.h"
//TODO: check why this is required
#include "gratingtexture.h"
#include "../../gratingwidget.h"
#include "../../pointwidget.h"

namespace latero::graphics {


class GratingTextureSeedCtrl : public Gtk::Frame
{
public:
	GratingTextureSeedCtrl(GratingTexturePtr peer);
	virtual ~GratingTextureSeedCtrl() {};
protected:
	void OnChanged();
	GratingTexturePtr peer_;
	PointWidget widget_;
};

class GratingTextureWidgetSet
{
public:
	GratingTextureWidgetSet(GratingTexturePtr peer);
	virtual ~GratingTextureWidgetSet() {};

	PatternPreview preview_;
	OscillatorWidget vibCtrl_;
	TextureInvertCtrl invertCtrl_;
	TextureTDCentricCtrl tdCentricCtrl_;
	TextureAmplitudeCtrl ampCtrl_;
	GratingTextureSeedCtrl seedCtrl_;
	GratingWidgetSet gratingCtrls_;
};


class GratingTextureWidget : public TextureWidget
{
public:
	GratingTextureWidget(GratingTexturePtr peer) : TextureWidget(peer), peer_(peer) {};
	virtual ~GratingTextureWidget() {};

protected:
	GratingRidgeSizeWidget* CreateRidgeSizeWidget();
	GratingGapSizeWidget* CreateGapSizeWidget();
	GratingVelocityWidget* CreateGratingVelocityWidget();
	GratingTextureSeedCtrl* CreateSeedWidget();
	GratingRegularizeButton *CreateRegularizeButton();

	GratingTexturePtr peer_;
};

} // namespace

