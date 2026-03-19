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

#include "imagewidget.h"
#include "image.h"

namespace latero {
namespace graphics { 

ImageWidget::ImageWidget(ImagePtr peer, bool showMaskWidget) :
	Gtk::Box(Gtk::Orientation::VERTICAL),
	txWidget_(peer->GetTexture()),
	maskWidget_(peer->GetMask(),peer->Dev()),
	peer_(peer)
{
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	append(*box);
	box->set_vexpand();
	
	if (showMaskWidget)
	{
		Gtk::Notebook *notebook = Gtk::manage(new Gtk::Notebook);
		notebook->append_page(maskWidget_, "mask");
		notebook->append_page(txWidget_, "texture");
		box->append(*notebook);
		notebook->set_hexpand();
	}
	else
	{
		box->append(txWidget_);
		txWidget_.set_hexpand();
	}
	//box->append(vizButton_);


	txWidget_.SignalTextureChanged().connect(
		sigc::mem_fun(*this, &ImageWidget::OnTextureChanged));
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::OnTextureChanged()
{
	peer_->SetTexture(txWidget_.GetTexture());
}

} // namespace graphics
} // namespace latero
