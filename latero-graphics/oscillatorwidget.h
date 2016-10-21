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

#ifndef LATERO_GRAPHICS_PLANAR_OSCILLATOR_WIDGET
#define LATERO_GRAPHICS_PLANAR_OSCILLATOR_WIDGET

#include <gtkmm.h>
#include "gtk/checkframe.h"
#include "oscillatorfwd.h"

namespace latero {
namespace graphics { 

class OscillatorEnableCheck : public Gtk::CheckButton
{
public:
	OscillatorEnableCheck(OscillatorPtr peer);
	virtual ~OscillatorEnableCheck() {};
protected:
	virtual void on_clicked();
	OscillatorPtr peer_;
};


class OscillatorAmplitudeCtrl : public Gtk::VBox
{
public:
	OscillatorAmplitudeCtrl(OscillatorPtr peer);
	virtual ~OscillatorAmplitudeCtrl() {};
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	OscillatorPtr peer_;
};


class OscillatorFreqCtrl : public Gtk::VBox
{
public:
	OscillatorFreqCtrl(OscillatorPtr peer);
	virtual ~OscillatorFreqCtrl() {};
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	OscillatorPtr peer_;
};


class OscillatorWidget : public gtk::CheckFrame
{
public:
	OscillatorWidget(OscillatorPtr peer, bool showBlendBode = false);
	virtual ~OscillatorWidget() {};
protected:
	void OnClick();
	OscillatorPtr peer_;
};

} // namespace graphics
} // namespace latero

#endif
