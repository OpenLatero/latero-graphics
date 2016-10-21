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

#include "patterncreatordialog.h"
#include "line.h"
#include "dot.h"
#include "dots.h"
#include "circle.h"
#include "polygon.h"
#include "image.h"
#include "texture/texture.h"
#include "dots.h"
#include <gtkmm/filechooserdialog.h>

namespace latero {
namespace graphics { 

PatternCreatorDialog::PatternCreatorDialog(const latero::Tactograph *dev) : 
	Gtk::Dialog("Create Pattern"), txCombo_(dev), dev_(dev)
{
	combo_.append_text("load from file");
	combo_.append_text("dot");
	combo_.append_text("dot set");
	combo_.append_text("line");
	combo_.append_text("circle");
	combo_.append_text("polygon");
	combo_.append_text("image");
	combo_.append_text("texture");
	combo_.append_text("group");

	combo_.set_active_text("line");
	txCombo_.set_sensitive(false);

	get_vbox()->pack_start(combo_);
	get_vbox()->pack_start(txCombo_);

	combo_.signal_changed().connect( sigc::mem_fun(*this, &PatternCreatorDialog::OnComboChanged) );
	
	add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	show_all_children();		
}


void PatternCreatorDialog::OnComboChanged()
{
	txCombo_.set_sensitive(combo_.get_active_text() == "texture");
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
		Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_SAVE);
		
		std::string dir = Glib::get_current_dir();

		Gtk::FileFilter filter;
		filter.add_pattern("*.pattern");
		filter.add_pattern("*.tx");
		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_CANCEL);
		std::string file = "new.pattern";
		dialog.set_current_name(file);
		dialog.add_filter(filter);
		if (Gtk::RESPONSE_OK == dialog.run())
			return Pattern::Create(dev_,dialog.get_filename()); 
		else
			return PatternPtr();
	}
	else return PatternPtr();
}


} // namespace graphics
} // namespace latero
