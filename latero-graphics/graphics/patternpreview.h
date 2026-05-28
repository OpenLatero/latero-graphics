#pragma once

#include <gtkmm.h>
#include "pattern.h"

namespace latero::graphics {

class PatternPreview : public Gtk::Box
{
public:
	static const Pattern::VizMode viz_mode = Pattern::viz_deflection_and_vibration;
	static const int height = 200;

	PatternPreview(PatternPtr peer);
	virtual ~PatternPreview() { if (popupMenu_) popupMenu_->unparent(); }
protected:
	void CreatePopupMenu();
	void OnSave();
	void OnClick(int n_press, double x, double y);
	bool OnTimer();
	void Refresh();

	boost::posix_time::ptime refreshTime_;
	Gtk::Picture img_;
	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	PatternPtr peer_;
};

} // namespace

