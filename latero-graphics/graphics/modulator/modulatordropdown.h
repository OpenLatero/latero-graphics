#pragma once

#include "modulatorfwd.h"
#include <gtkmm.h>
#include <latero/tactograph.h>

namespace latero::graphics {

class ModulatorDropDown : public Gtk::Box
{
public:
	ModulatorDropDown(ModulatorPtr tx);
	virtual ~ModulatorDropDown() {};
	void Append(ModulatorPtr mod);
	void SetActive(ModulatorPtr mod);
	sigc::signal<void(ModulatorPtr)> SignalModulatorChanged() { return signalModulatorChanged_; }

protected:
	sigc::signal<void(ModulatorPtr)> signalModulatorChanged_;
	void OnDropDownChanged();

	const latero::Tactograph *dev_; // tmp?
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	std::vector<ModulatorPtr> modList_;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> imgList_;
	bool signalEnable_;
};

} // namespace

