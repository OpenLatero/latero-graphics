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

#include "modulatorpreview.h"
#include "modulator.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/menu.h>

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

	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	group->add(Gtk::Action::create("ContextSaveAs", "save as", "save image as"),
		sigc::mem_fun(*this, &ModulatorPreview::OnSaveAs));
	group->add(Gtk::Action::create("ContextSave", "save", "save image"),
		sigc::mem_fun(*this, &ModulatorPreview::OnSave));

	uiManager_ = Gtk::UIManager::create();
	uiManager_->insert_action_group(group);

	std::stringstream buf;
	buf << "<ui>";
	buf << "  <popup name='PopupMenu'>";
	buf << "     <menuitem action='ContextSaveAs'/>";
	buf << "     <menuitem action='ContextSave'/>";
	buf << "  </popup>";
	buf << "</ui>";
	uiManager_->add_ui_from_string(buf.str());
}

void ModulatorPreview::OnSaveAs()
{
	Gtk::FileChooserDialog dialog("Please select file name.", Gtk::FILE_CHOOSER_ACTION_SAVE);
	std::string dir = Glib::get_current_dir();
	dialog.set_current_folder(dir);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
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
		((Gtk::Menu*)uiManager_->get_widget("/PopupMenu"))->popup(event->button, event->time);
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
