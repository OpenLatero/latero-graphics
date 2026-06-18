#pragma once

#include <gtkmm.h>
#include "velmodulator.h"

namespace latero::graphics {

class VelModulatorWidget : public Gtk::Box
{
public:
	VelModulatorWidget(VelModulatorPtr peer);
	virtual ~VelModulatorWidget() {};
protected:
	VelModulatorPtr peer_;
};

} // namespace

