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
#include "patterncreatordialog.h"
#include "line.h"
#include "dot.h"
#include "dots.h"
#include "circle.h"
#include "polygon.h"
#include "image.h"
#include "texture/texture.h"
#include "dots.h"

namespace latero {
namespace graphics { 

PatternCreatorDialog::PatternCreatorDialog(const latero::Tactograph *dev) : 
	Gtk::Dialog("Create Pattern"), txCombo_(dev), dev_(dev)
{
	combo_.append("load from file");
	combo_.append("dot");
	combo_.append("dot set");
	combo_.append("line");
	combo_.append("circle");
	combo_.append("polygon");
	combo_.append("image");
	combo_.append("texture");
	combo_.append("group");

	combo_.set_active_text("line");
	txCombo_.set_sensitive(false);

	get_content_area()->append(combo_);
	get_content_area()->append(txCombo_);

	combo_.set_vexpand();
	txCombo_.set_vexpand();

	combo_.signal_changed().connect( sigc::mem_fun(*this, &PatternCreatorDialog::OnComboChanged) );
	
	add_button("Ok", Gtk::ResponseType::OK);
	add_button("Cancel", Gtk::ResponseType::CANCEL);
}


void PatternCreatorDialog::OnComboChanged()
{
	txCombo_.set_sensitive(combo_.get_active_text() == "texture");

	// GTKMM4: show file dialog immediately on selection so the path is ready when OK is clicked
	if (combo_.get_active_text() == "load from file")
	{
		loadedFile_.clear();
		auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::OPEN);
		auto filter = Gtk::FileFilter::create();
		filter->add_pattern("*.pattern");
		filter->add_pattern("*.tx");
		dialog->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
		dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
		dialog->add_button("Open", Gtk::ResponseType::OK);
		dialog->set_default_response(Gtk::ResponseType::CANCEL);
		dialog->add_filter(filter);
		dialog->set_transient_for(*this);
		dialog->signal_response().connect([this, dialog](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
				loadedFile_ = dialog->get_file()->get_path();
			delete dialog;
		});
		dialog->show();
	}
}

PatternPtr PatternCreatorDialog::CreatePattern()
{
	std::string type = combo_.get_active_text();
	if (type == "dot")		return Dot::Create(dev_);
	else if (type == "dot set")	return Dots::Create(dev_);
	else if (type == "line")	return Line::Create(dev_);
	else if (type == "circle")	return Circle::Create(dev_);
	else if (type == "polygon")	return Polygon::Create(dev_);
	else if (type == "image")	return Image::Create(dev_);
	else if (type == "texture")	return txCombo_.GetTexture();
	else if (type == "group")	return Group::Create(dev_);
	else if (type == "load from file")
	{
		// GTKMM4: file was already selected in OnComboChanged(); path stored in loadedFile_
		if (!loadedFile_.empty())
			return Pattern::Create(dev_, loadedFile_);
		return PatternPtr();
	}
	else return PatternPtr();
}


} // namespace graphics
} // namespace latero
