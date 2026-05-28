#pragma once

#include <gtkmm.h>

namespace latero::graphics::gtk {

class CheckFrame : public Gtk::Frame
{
public:
	CheckFrame(bool checkState, std::string label = "", bool check_in_label = false);
	virtual ~CheckFrame() {};

	Gtk::CheckButton &GetCheck() { return check_; }
	Gtk::Box &GetBox() { return innerBox_; }

protected:
	void OnClick() { innerBox_.set_sensitive(check_.get_active()); }

	Gtk::Box mainBox_;
	Gtk::Box innerBox_;
	Gtk::CheckButton check_;
};

} // namespace

