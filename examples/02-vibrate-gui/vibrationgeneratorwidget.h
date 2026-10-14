#pragma once

#include <gtkmm.h>
#include "vibrationgenerator.h"

class VibrationGeneratorWidget : public Gtk::Box
{
public:
	VibrationGeneratorWidget(VibrationGeneratorPtr peer);
	virtual ~VibrationGeneratorWidget() {};
protected:
	void OnChanged();
	Glib::RefPtr<Gtk::Adjustment> adj_;
	VibrationGeneratorPtr peer_;
};
