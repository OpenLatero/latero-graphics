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

#include "imagecombo.h"
#include <iostream>

namespace latero {
namespace graphics { 

namespace gtk {

ImageCombo::ImageCombo()
{
	model_ = Gtk::ListStore::create(columns_);
	combo_.set_model(model_);
	combo_.pack_start(columns_.img_);
	add(combo_);
	combo_.signal_changed().connect( sigc::mem_fun(*this, &ImageCombo::OnComboChanged) );
	show_all_children();
}

ImageCombo::~ImageCombo()
{
}

void ImageCombo::Append(int id, std::string imgfile)
{
	try
	{
		Gtk::TreeModel::Row row = *(model_->append());
		row[columns_.id_] = id;
		row[columns_.img_] = Gdk::Pixbuf::create_from_file(imgfile);
	}
	catch (Glib::Error &e)
	{
		std::cerr << "ImageCombo::Append() " << e.what() << "\n";
	}
}

void ImageCombo::SetActive(int id)
{
	Gtk::TreeModel::Children::iterator iter;
	for (iter = model_->children().begin(); iter != model_->children().end(); iter++)
	{
		if ((*iter)[columns_.id_] == id)
		{
			combo_.set_active(iter);
		}
	}
}

void ImageCombo::OnComboChanged()
{
	Gtk::TreeModel::iterator iter = combo_.get_active();
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if(row)
		{
			SignalChanged()(row[columns_.id_]);
		}
	}
}

sigc::signal<void,int> ImageCombo::SignalChanged()
{
	return signalChanged_;
};

} // namespace gtk

} // namespace graphics
} // namespace latero
