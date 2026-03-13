// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#include <filesystem>
#include "modulatorpreview.h"
#include "modulator.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/menu.h>
#include <gtkmm/builder.h>
#include <glibmm/main.h>
#include <giomm/simpleactiongroup.h>

namespace latero {
namespace graphics { 

ModulatorPreview::ModulatorPreview(ModulatorPtr peer) : peer_(peer)
{
	add(img_);
	Refresh();
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &ModulatorPreview::OnTimer),
		(uint)333, // ms
		Glib::PRIORITY_DEFAULT_IDLE);
	CreatePopupMenu();
}

void ModulatorPreview::CreatePopupMenu()
{
	set_events(Gdk::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &ModulatorPreview::OnClick));

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("saveas", sigc::mem_fun(*this, &ModulatorPreview::OnSaveAs));
	action_group->add_action("save",   sigc::mem_fun(*this, &ModulatorPreview::OnSave));
	insert_action_group("modulator", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  	<menu id="PopupMenu">
    <item>
      <attribute name="label">save as</attribute>
      <attribute name="action">modulator.saveas</attribute>
    </item>
    <item>
      <attribute name="label">save</attribute>
      <attribute name="action">modulator.save</attribute>
    </item>
  	</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = Glib::RefPtr<Gio::Menu>::cast_dynamic(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::Menu>(menu_model);
	popupMenu_->attach_to_widget(*this);
}

void ModulatorPreview::OnSaveAs()
{
	Gtk::FileChooserDialog dialog("Please select file name.", Gtk::FILE_CHOOSER_ACTION_SAVE);
	std::string dir = std::filesystem::current_path().string();
	dialog.set_current_folder(dir);
	dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Save", Gtk::RESPONSE_OK);
	dialog.set_default_response(Gtk::RESPONSE_CANCEL);
	dialog.set_current_name("modulation.png");
	if (Gtk::RESPONSE_OK == dialog.run())
	{
		printf("saving %s\n", dialog.get_filename().c_str());
		peer_->GetIllustration(1000,50)->save(dialog.get_filename(),"png");
	}
}

void ModulatorPreview::OnSave()
{
	printf("saving modulator.png\n");
	peer_->GetIllustration(1000,50)->save("modulator.png","png");
}

	
bool ModulatorPreview::OnClick(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		popupMenu_->popup_at_pointer((GdkEvent*)event);
		return true;
	}
	else
		return false;
}

bool ModulatorPreview::OnTimer()
{
	if (peer_->GetLastModified() > refreshTime_)
		Refresh();
	return true;
}

void ModulatorPreview::Refresh()
{
	img_.set(peer_->GetIllustration(200,50));
	refreshTime_ = boost::posix_time::microsec_clock::universal_time();
}

} // namespace graphics
} // namespace latero
