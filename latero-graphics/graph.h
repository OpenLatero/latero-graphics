#pragma once

#include "gtk/plot.h"
#include <gtkmm.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

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

	boost::posix_time::ptime refreshTime_; // time of the last refresh
	uint nbPoints_;
	latero::graphics::gtk::Plot plot_;
};

} // namespace

