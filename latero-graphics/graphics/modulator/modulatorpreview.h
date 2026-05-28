#pragma once

#include "modulatorfwd.h"
#include <gtkmm.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero::graphics {

class ModulatorPreview : public Gtk::Box
{
public:
	ModulatorPreview(ModulatorPtr peer);
	virtual ~ModulatorPreview() { if (popupMenu_) popupMenu_->unparent(); }
protected:
	void CreatePopupMenu();
	void OnSave();
	void OnSaveAs();
	void OnClick(int n_press, double x, double y);
	bool OnTimer();
	void Refresh();

	boost::posix_time::ptime refreshTime_;
	Gtk::Image img_;
	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	ModulatorPtr peer_;
};

} // namespace

