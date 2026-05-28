#pragma once

#include "pattern.h"
#include <gtkmm.h>
#include "texture/texturedropdown.h"

namespace latero::graphics {

class PatternCreatorDialog : public Gtk::Window
{
public:
	PatternCreatorDialog(const latero::Tactograph *dev);
	virtual ~PatternCreatorDialog() {};

	void OnDropDownChanged();
	PatternPtr CreatePattern();
	sigc::signal<void(int)>& signal_response() { return signalResponse_; }

protected:
	Glib::RefPtr<Gtk::StringList> typeList_;
	Gtk::DropDown* typeDropDown_;
	TextureDropDown txDropDown_;
	const latero::Tactograph *dev_;
	std::string loadedFile_;
	sigc::signal<void(int)> signalResponse_;
};

} // namespace

