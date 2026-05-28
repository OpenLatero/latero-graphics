#pragma once

#include <gtkmm.h>
#include "point.h"

namespace latero::graphics {

class PointWidget : public Gtk::Box
{
public:
	PointWidget(const Point &init, 
		double xlim_min=-100000, double xlim_max=100000, 
		double ylim_min=-100000, double ylim_max=100000,
		bool showSliders = true);
	Point GetValue();
	void SetValue(const Point &p);
	sigc::signal<void()> SignalValueChanged();
protected:
	sigc::signal<void()> signalValueChanged_;
    Glib::RefPtr<Gtk::Adjustment> xAdj_, yAdj_;
};

} // namespace

