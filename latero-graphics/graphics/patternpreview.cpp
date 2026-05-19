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
#include <sys/stat.h>
#include "patternpreview.h"
#include <iostream>

namespace latero::graphics {

/*
// TODO: add options for size
class PatternIllustrationSaveDialog : public Gtk::FileChooserDialog
{
public:
	PatternIllustrationSaveDialog();
	virtual ~PatternIllustrationSaveDialog() {}
};

// TODO: add options for size
class PatternThumbnailSaveDialog : public Gtk::FileChooserDialog
{
public:
	PatternThumbnailSaveDialog();
	virtual ~PatternThumbnailSaveDialog() {}
};


PatternIllustrationSaveDialog::PatternIllustrationSaveDialog() :
	Gtk::FileChooserDialog("Please select file name.", Gtk::FileChooser::Action::SAVE)
{
	std::string dir = std::filesystem::current_path().string();
	set_current_folder(Gio::File::create_for_path(dir));
	add_button("Cancel", Gtk::ResponseType::CANCEL);
	add_button("Save", Gtk::ResponseType::OK);
	set_default_response(Gtk::ResponseType::CANCEL);
	set_current_name("pattern.png");
}

PatternThumbnailSaveDialog::PatternThumbnailSaveDialog() :
	Gtk::FileChooserDialog("Please select file name.", Gtk::FileChooser::Action::SAVE)
{
	std::string dir = std::filesystem::current_path().string();
	set_current_folder(Gio::File::create_for_path(dir));
	add_button("Cancel", Gtk::ResponseType::CANCEL);
	add_button("Save", Gtk::ResponseType::OK);
	set_default_response(Gtk::ResponseType::CANCEL);
	set_current_name("thumbnail.png");
}
*/

PatternPreview::PatternPreview(PatternPtr peer) :
	peer_(peer),
	refreshTime_(boost::posix_time::min_date_time),
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	append(img_);
	img_.set_can_shrink(true);
	Refresh();
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &PatternPreview::OnTimer),
		(uint)333, // ms
		Glib::PRIORITY_DEFAULT_IDLE);
	CreatePopupMenu();
}

void PatternPreview::CreatePopupMenu()
{
	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &PatternPreview::OnClick));
	add_controller(gesture);

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("save", sigc::mem_fun(*this, &PatternPreview::OnSave));
	insert_action_group("pattern", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  	<menu id="PopupMenu">
    	<item>
      	<attribute name="label">save</attribute>
      	<attribute name="action">pattern.save</attribute>
    	</item>
  	</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu_model);
	popupMenu_->set_parent(*this);
}

void PatternPreview::OnSave()
{
	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select file name.");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_initial_name("pattern.png");
	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->save(*win, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto file = dialog->save_finish(result);
			std::string filename = file->get_path();
			peer_->GetVisualization(1000,boost::posix_time::seconds(0),viz_mode)->save(filename,"png");
			chmod(filename.c_str(), 0666);
		} catch (const Gtk::DialogError&) {}
	});
}


	
void PatternPreview::OnClick(int n_press, double x, double y)
{
	popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
	popupMenu_->popup();
}

bool PatternPreview::OnTimer()
{
	if (peer_->GetLastModified() > refreshTime_)
		Refresh();
	return true;
}

void PatternPreview::Refresh()
{
	int w = height*peer_->Dev()->GetSurfaceWidth()/peer_->Dev()->GetSurfaceHeight();
	img_.set_pixbuf(peer_->GetVisualization(w,boost::posix_time::seconds(0),viz_mode));
	refreshTime_ = boost::posix_time::microsec_clock::universal_time();
}

} // namespace

