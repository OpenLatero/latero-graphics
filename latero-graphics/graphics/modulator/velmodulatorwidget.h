#pragma once

#include <gtkmm.h>
#include "velmodulatorfwd.h"

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

