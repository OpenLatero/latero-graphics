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
	txWidget_(peer->GetTexture()),
	maskWidget_(peer->GetMask(),peer->Dev()),
	peer_(peer)
{
	Gtk::HBox *box = manage(new Gtk::HBox());
	pack_start(*box);
	
	if (showMaskWidget)
	{
		Gtk::Notebook *notebook = manage(new Gtk::Notebook);
		notebook->append_page(maskWidget_, "mask");
		notebook->append_page(txWidget_, "texture");
		box->pack_start(*notebook);
	}
	else
	{
		box->pack_start(txWidget_);
	}
	//box->pack_start(vizButton_,Gtk::PACK_SHRINK);


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
