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

#ifndef LATERO_GRAPHICS_GRATING_TEXTURE_WIDGET
#define LATERO_GRAPHICS_GRATING_TEXTURE_WIDGET

#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "texturewidget.h"
#include "../patternpreview.h"
//TODO: check why this is required
#include "gratingtexture.h"
#include "../../gratingwidget.h"
#include "../../pointwidget.h"

namespace latero {
namespace graphics { 


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

} // namespace graphics
} // namespace latero

#endif
