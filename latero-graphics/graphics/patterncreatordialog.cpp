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
	Gtk::Window(), txCombo_(dev), dev_(dev)
{
	set_title("Create Pattern");
	set_modal(true);
	set_size_request(300, -1);

	typeList_ = Gtk::StringList::create({});
	typeList_->append("load from file");
	typeList_->append("dot");
	typeList_->append("dot set");
	typeList_->append("line");
	typeList_->append("circle");
	typeList_->append("polygon");
	typeList_->append("image");
	typeList_->append("texture");
	typeList_->append("group");
	typeDropDown_ = Gtk::make_managed<Gtk::DropDown>(typeList_);
	typeDropDown_->set_selected(3); // "line"
	txCombo_.set_sensitive(false);

	auto okButton     = Gtk::make_managed<Gtk::Button>("Ok");
	auto cancelButton = Gtk::make_managed<Gtk::Button>("Cancel");
	auto bbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	bbox->set_halign(Gtk::Align::END);
	bbox->set_spacing(6);
	bbox->append(*okButton);
	bbox->append(*cancelButton);

	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	vbox->set_margin(12);
	vbox->set_spacing(6);
	vbox->append(*typeDropDown_);
	vbox->append(txCombo_);
	vbox->append(*bbox);
	set_child(*vbox);

	typeDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &PatternCreatorDialog::OnComboChanged));
	okButton->signal_clicked().connect([this]{
		signalResponse_.emit((int)Gtk::ResponseType::OK);
	});
	cancelButton->signal_clicked().connect([this]{
		signalResponse_.emit((int)Gtk::ResponseType::CANCEL);
	});
}


void PatternCreatorDialog::OnComboChanged()
{
	auto activeText = std::string(typeList_->get_string(typeDropDown_->get_selected()));
	txCombo_.set_sensitive(activeText == "texture");
	if (activeText == "load from file")
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
	std::string type = std::string(typeList_->get_string(typeDropDown_->get_selected()));
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
		if (!loadedFile_.empty())
			return Pattern::Create(dev_, loadedFile_);
		return PatternPtr();
	}
	else return PatternPtr();
}


} // namespace graphics
} // namespace latero
