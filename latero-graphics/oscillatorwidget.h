#pragma once

#include <gtkmm.h>
#include "gtk/checkframe.h"
#include "oscillator.h"

namespace latero::graphics {

class OscillatorEnableCheck : public Gtk::CheckButton
{
public:
	OscillatorEnableCheck(OscillatorPtr peer);
	virtual ~OscillatorEnableCheck() {};
protected:
	OscillatorPtr peer_;
};


class OscillatorAmplitudeCtrl : public Gtk::Box
{
public:
	OscillatorAmplitudeCtrl(OscillatorPtr peer);
	virtual ~OscillatorAmplitudeCtrl() {};
protected:
	void OnChanged();
    Glib::RefPtr<Gtk::Adjustment> adj_;
	OscillatorPtr peer_;
};


class OscillatorFreqCtrl : public Gtk::Box
{
public:
	OscillatorFreqCtrl(OscillatorPtr peer);
	virtual ~OscillatorFreqCtrl() {};
protected:
	void OnChanged();
    Glib::RefPtr<Gtk::Adjustment> adj_;
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

} // namespace

