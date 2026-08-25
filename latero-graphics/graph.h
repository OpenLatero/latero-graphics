#pragma once

#include "gtk/plot.h"
#include <gtkmm.h>
#include <chrono>

namespace latero::graphics {

class Graph : public Gtk::Box
{
public:
	Graph(uint nbPoints=1000, const char *fgColor = "blue", const char *bgColor = "white");
	virtual ~Graph();
	void Refresh();
	virtual void UpdateRange() {};
protected:
	virtual double GetValue(double x) = 0;

	std::chrono::steady_clock::time_point refreshTime_;
	uint nbPoints_;
	latero::graphics::gtk::Plot plot_;
};

} // namespace

