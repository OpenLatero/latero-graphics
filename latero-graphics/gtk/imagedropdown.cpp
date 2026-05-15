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

#include "imagedropdown.h"
#include <iostream>

namespace latero {
namespace graphics { 

namespace gtk {

ImageDropDown::ImageDropDown() :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	list_(Gtk::StringList::create({})),
	dropDown_(list_)
{
	auto factory = Gtk::SignalListItemFactory::create();
	factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		item->set_child(*Gtk::make_managed<Gtk::Image>());
	});
	factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& item) {
		auto pos = item->get_position();
		if (pos < items_.size())
			if (auto image = dynamic_cast<Gtk::Image*>(item->get_child()))
				image->set(items_[pos].second);
	});
	dropDown_.set_factory(factory);
	dropDown_.set_list_factory(factory);
	append(dropDown_);
	dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &ImageDropDown::OnDropDownChanged));
}

ImageDropDown::~ImageDropDown()
{
}

void ImageDropDown::Append(int id, std::string imgfile)
{
	try
	{
		items_.push_back({id, Gdk::Pixbuf::create_from_file(imgfile)});
		list_->append("");
	}
	catch (Glib::Error &e)
	{
		std::cerr << "ImageDropDown::Append() " << e.what() << "\n";
	}
}

void ImageDropDown::SetActive(int id)
{
	for (guint i = 0; i < items_.size(); ++i)
		if (items_[i].first == id) { dropDown_.set_selected(i); return; }
}

void ImageDropDown::OnDropDownChanged()
{
	guint pos = dropDown_.get_selected();
	if (pos < items_.size())
		signalChanged_(items_[pos].first);
}

sigc::signal<void(int)> ImageDropDown::SignalChanged()
{
	return signalChanged_;
};

} // namespace gtk

} // namespace graphics
} // namespace latero
