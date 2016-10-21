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

#include <sys/stat.h>
#include "patternpreview.h"
#include <gtkmm.h>

namespace latero {
namespace graphics { 

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
	Gtk::FileChooserDialog("Please select file name.", Gtk::FILE_CHOOSER_ACTION_SAVE)
{
	std::string dir = Glib::get_current_dir();
	set_current_folder(dir);
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	set_default_response(Gtk::RESPONSE_CANCEL);
	set_current_name("pattern.png");
}

PatternThumbnailSaveDialog::PatternThumbnailSaveDialog() :
	Gtk::FileChooserDialog("Please select file name.", Gtk::FILE_CHOOSER_ACTION_SAVE)
{
	std::string dir = Glib::get_current_dir();
	set_current_folder(dir);
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	set_default_response(Gtk::RESPONSE_CANCEL);
	set_current_name("thumbnail.png");
}

PatternPreview::PatternPreview(PatternPtr peer) : peer_(peer)
{
	add(img_);
	Refresh();
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &PatternPreview::OnTimer),
		(uint)333, // ms
		Glib::PRIORITY_DEFAULT_IDLE);
	CreatePopupMenu();
}

void PatternPreview::CreatePopupMenu()
{
	set_events(Gdk::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &PatternPreview::OnClick));

	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	group->add(Gtk::Action::create("ContextSave", "save", "save image"),
		sigc::mem_fun(*this, &PatternPreview::OnSave));


	uiManager_ = Gtk::UIManager::create();
	uiManager_->insert_action_group(group);

	std::stringstream buf;
	buf << "<ui>";
	buf << "  <popup name='PopupMenu'>";
	buf << "     <menuitem action='ContextSave'/>";
	buf << "     <menuitem action='ContextSaveThumbnail'/>";
	buf << "  </popup>";
	buf << "</ui>";
	uiManager_->add_ui_from_string(buf.str());
}

void PatternPreview::OnSave()
{
	PatternIllustrationSaveDialog dialog;
	if (Gtk::RESPONSE_OK == dialog.run())
	{
		printf("saving pattern preview to %s\n", dialog.get_filename().c_str());
		peer_->GetVisualization(1000,boost::posix_time::seconds(0),viz_mode)->save(dialog.get_filename(),"png");
		chmod(dialog.get_filename().c_str(), 0666); // make accessible to others than root
	}
}


	
bool PatternPreview::OnClick(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		((Gtk::Menu*)uiManager_->get_widget("/PopupMenu"))->popup(event->button, event->time);
		return true;
	}
	else
		return false;
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
	img_.set(peer_->GetVisualization(w,boost::posix_time::seconds(0),viz_mode));
	refreshTime_ = boost::posix_time::microsec_clock::universal_time();
}

} // namespace graphics
} // namespace latero

