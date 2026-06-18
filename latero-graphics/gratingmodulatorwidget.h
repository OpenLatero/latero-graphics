#pragma once

#include <gtkmm.h>
#include "gratingmodulator.h"

namespace latero::graphics {

class GratingModulatorCtrl : public Gtk::Frame
{
public:
	GratingModulatorCtrl(GratingModulatorPtr peer);
	virtual ~GratingModulatorCtrl() {};
protected:
	void OnClick();
	Gtk::Box widgetbox_;
	Gtk::CheckButton check_;
	GratingModulatorPtr peer_;
};

} // namespace

