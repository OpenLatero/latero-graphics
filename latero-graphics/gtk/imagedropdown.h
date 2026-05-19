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

#pragma once

#include <gtkmm.h>

namespace latero::graphics::gtk {

/**
 * This widget implements a DropDown that displays a list of images. A numerical ID is associated with
 * each image.
 */
class ImageDropDown : public Gtk::Box
{
public:
	/** ctor */
	ImageDropDown();

	/** dtor */
	virtual ~ImageDropDown();

	/** Append an image with its associated ID. */
	void Append(int id, std::string imgfile);

	/** Set activce the image with a given ID. */
	void SetActive(int id);

	/** Callback for changes to selected image. Image ID is passed as an argument. */
	sigc::signal<void(int)> SignalChanged();

protected:

	void OnDropDownChanged();

	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	std::vector<std::pair<int, Glib::RefPtr<Gdk::Pixbuf>>> items_;
	sigc::signal<void(int)> signalChanged_;
};

} // namespace

