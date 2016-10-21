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

#ifndef LATERO_GRAPHICS_PLANAR_TEXTURE_WIDGET
#define LATERO_GRAPHICS_PLANAR_TEXTURE_WIDGET

#include "texturecombo.h"
#include "texturefwd.h"
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/expander.h>
#include "../../pointwidget.h"
#include "../../gtk/checkframe.h"
#include "../../oscillatorwidget.h"

namespace latero {
namespace graphics { 

class CreateTextureDlg : public Gtk::Dialog
{
public:
	CreateTextureDlg(const latero::Tactograph *dev);
	virtual ~CreateTextureDlg() {}
	void OnComboChanged();
	TexturePtr CreateTexture();
protected:
	Gtk::ComboBoxText combo_;
	TextureCombo txCombo_;
	const latero::Tactograph *dev_;
};


class TextureTDCentricCheck : public Gtk::CheckButton
{
public:
	TextureTDCentricCheck(TexturePtr peer);
	virtual ~TextureTDCentricCheck() {};
protected:
	virtual void on_clicked();
	TexturePtr peer_;
};

class TextureTDCentricCtrl : public Gtk::Frame
{
public:
	TextureTDCentricCtrl(TexturePtr peer);
	virtual ~TextureTDCentricCtrl() {};
protected:
	void OnClick();
	void OnPosChanged();
	PointWidget point_;
	TexturePtr peer_;
};

class TextureAmplitudeCtrl : public Gtk::VBox
{
public:
	TextureAmplitudeCtrl(TexturePtr peer);
	virtual ~TextureAmplitudeCtrl() {};
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	TexturePtr peer_;
};

class TextureInvertCtrl : public Gtk::HBox
{
public:
	TextureInvertCtrl(TexturePtr peer);
	virtual ~TextureInvertCtrl() {};
protected:
	void OnClick();
	Gtk::CheckButton check_;
	TexturePtr peer_;
};

class TextureMotionCtrl : public gtk::CheckFrame
{
public:
	TextureMotionCtrl(TexturePtr peer);
	virtual ~TextureMotionCtrl() {};
protected:
	void OnClick();
	TexturePtr peer_;
};

class TextureAdvancedButton : public Gtk::Button
{
public:
	TextureAdvancedButton(TexturePtr peer);
	virtual ~TextureAdvancedButton() {};
	sigc::signal<void> SignalClosed() { return signalClosed_; }
protected:
	void OnResponse(int i) { signalClosed_(); }
	virtual void on_clicked();

	sigc::signal<void> signalClosed_;
    Gtk::Widget *adv_;
	Gtk::Dialog dlg_;
	TexturePtr peer_;	
};


class TextureWidget : public Gtk::Frame
{
public:
	TextureWidget(TexturePtr peer);
	virtual ~TextureWidget();
protected:

	TextureTDCentricCheck* CreateTDCentricCheck();
	OscillatorEnableCheck* CreateVibCheck();
	TextureMotionCtrl* CreateMotionWidget();

	void SetContent(Gtk::Widget *widget, bool showPanel=true, bool showPreview=true, bool showAdvanced=true);
	void OnAdvanced();
	void OnAdvancedClosed();
	virtual void Rebuild() = 0;
	Gtk::Widget *CreateLeftPanel();

	TexturePtr peer_;

};

} // namespace graphics
} // namespace latero

#endif
