#include "checkframe.h"

namespace latero::graphics::gtk {

CheckFrame::CheckFrame(bool checkState, std::string label, bool check_in_label) :
	Gtk::Frame(label), 	mainBox_(Gtk::Orientation::HORIZONTAL), innerBox_(Gtk::Orientation::HORIZONTAL)
{
	if (check_in_label)
	{
		check_.set_label(label);
		set_label_widget(check_);
	}

	check_.set_active(checkState);
	innerBox_.set_sensitive(checkState);

	set_child(mainBox_);
	if (!check_in_label) mainBox_.append(check_);
	mainBox_.append(innerBox_);
	innerBox_.set_hexpand();
	check_.signal_toggled().connect(sigc::mem_fun(*this, &CheckFrame::OnClick));
}


} // namespace

