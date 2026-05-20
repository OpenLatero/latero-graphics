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

namespace latero::graphics {

PatternCreatorDialog::PatternCreatorDialog(const latero::Tactograph *dev) :
	Gtk::Window(), txDropDown_(dev), dev_(dev)
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
	txDropDown_.set_sensitive(false);

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
	vbox->append(txDropDown_);
	vbox->append(*bbox);
	set_child(*vbox);

	typeDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &PatternCreatorDialog::OnDropDownChanged));
	okButton->signal_clicked().connect([this]{
		signalResponse_.emit((int)Gtk::ResponseType::OK);
	});
	cancelButton->signal_clicked().connect([this]{
		signalResponse_.emit((int)Gtk::ResponseType::CANCEL);
	});
}


void PatternCreatorDialog::OnDropDownChanged()
{
	auto activeText = std::string(typeList_->get_string(typeDropDown_->get_selected()));
	txDropDown_.set_sensitive(activeText == "texture");
	if (activeText == "load from file")
	{
		loadedFile_.clear();
		auto filter = Gtk::FileFilter::create();
		filter->set_name("pattern files");
		filter->add_suffix("pattern");
		filter->add_suffix("tx");
		auto filters = Gio::ListStore<Gtk::FileFilter>::create();
		filters->append(filter);
		auto dialog = Gtk::FileDialog::create();
		dialog->set_title("Please select a file...");
		dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
		dialog->set_filters(filters);
		dialog->set_default_filter(filter);
		dialog->open(*this, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
			try {
				auto file = dialog->open_finish(result);
				loadedFile_ = file->get_path();
			} catch (const Gtk::DialogError&) {}
		});
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
	else if (type == "texture")	return txDropDown_.GetTexture();
	else if (type == "group")	return Group::Create(dev_);
	else if (type == "load from file")
	{
		if (!loadedFile_.empty())
			return Pattern::Create(dev_, loadedFile_);
		return PatternPtr();
	}
	else return PatternPtr();
}


} // namespace
