#pragma once

#include <gtkmm.h>
#include "dirmodulator.h"

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

