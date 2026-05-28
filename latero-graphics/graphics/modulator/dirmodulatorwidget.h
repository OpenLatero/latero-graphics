#pragma once

#include <gtkmm.h>
#include "dirmodulatorfwd.h"

namespace latero::graphics {

class DirModulatorWidget : public Gtk::Box
{
public:
	DirModulatorWidget(DirModulatorPtr peer);
	virtual ~DirModulatorWidget() {};
protected:
	DirModulatorPtr peer_;
};

} // namespace

