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

#include "texturecombo.h"
#include "stock.h"
#include "texture.h"
#include "lineargratingtexture.h"
#include "radialgratingtexture.h"
#include "axialgratingtexture.h"
#include "combotexture.h"
#include "vibrotexture.h"
#include "nulltexture.h"

namespace latero {
namespace graphics { 

TextureCombo::TextureCombo(TexturePtr tx, std::vector<std::string> textures) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	signalEnable_(true), textures_(textures), dev_(tx->Dev())
{
	if (textures_.size()==0) textures_ = GetStockTextures();

	model_ = Gtk::ListStore::create(columns_);
	combo_.set_model(model_);
	Gtk::CellRendererPixbuf* cell = Gtk::make_managed<Gtk::CellRendererPixbuf>();
	combo_.pack_start(*cell);
	combo_.add_attribute(*cell, "pixbuf", columns_.img);
	append(combo_);

	for (unsigned int i=0; i<textures_.size(); ++i)
		Append(textures_[i]);

	SetActive(tx);

	combo_.signal_changed().connect( sigc::mem_fun(*this, &TextureCombo::OnComboChanged) );
}

TextureCombo::TextureCombo(const latero::Tactograph *dev, std::vector<std::string> textures) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	signalEnable_(true), textures_(textures), dev_(dev)
{
	if (textures_.size()==0) textures_ = GetStockTextures();

	model_ = Gtk::ListStore::create(columns_);
	combo_.set_model(model_);
	Gtk::CellRendererPixbuf* cell = Gtk::make_managed<Gtk::CellRendererPixbuf>();
	combo_.pack_start(*cell);
	combo_.add_attribute(*cell, "pixbuf", columns_.img);
	append(combo_);

	for (unsigned int i=0; i<textures_.size(); ++i)
		Append(textures_[i]);

	SetActive(Texture::Create(dev,tx_grating_vertical));

	combo_.signal_changed().connect( sigc::mem_fun(*this, &TextureCombo::OnComboChanged) );
}

void TextureCombo::Append(std::string txfile)
{
	TexturePtr tx = Texture::Create(dev_,txfile);
	Gtk::TreeModel::Row row = *(model_->append());
	row[columns_.imgfile] = tx->GetIconFile();

	Glib::RefPtr<Gdk::Pixbuf> img  = Gdk::Pixbuf::create_from_file(row[columns_.imgfile]);
	if ((img->get_width() != 50) || (img->get_height() != 50))
		img = img->scale_simple(50,50,Gdk::InterpType::BILINEAR);
	row[columns_.img] = img;

	txFileList_.push_back(txfile);
}

void TextureCombo::SetActive(TexturePtr tx)
{
	if (!tx) return;
	tx_ = tx;
	std::string txfile = tx->GetXMLFile();

	// check if the txfile is already in the tree
	for (unsigned int i=0; i<txFileList_.size(); ++i)
	{
		if (txFileList_[i] == txfile)
		{
			signalEnable_ = false;
			combo_.set_active(i); //  don't reload!
			signalEnable_ = true;
			return;
		}
	}

	// Try to find a matching icon file
	std::string iconfile = tx->GetDefaultIconFile();
	Gtk::TreeModel::Children::iterator iter;
	for (iter = model_->children().begin(); iter != model_->children().end(); iter++)
	{
		std::string imgfile = (*iter)[columns_.imgfile];
		if (imgfile == iconfile)
		{
			signalEnable_ = false;
			combo_.set_active(iter); //  don't reload!
			signalEnable_ = true;
			return;
		}
	}

	// Last resort... add a new icon
	if (txfile=="")
	{
 		// this texture was not loaded from a file, let's create a temporary file
		char file[255];
		snprintf(file, sizeof(file), ".tmp-%d.tx", rand()%1024);
		tx->Save(file); // TODO: make sure we don't overwrite an existing file
	}
	Append(tx->GetXMLFile());
	SetActive(tx);
}



void TextureCombo::OnComboChanged()
{
	int curIndex = combo_.get_active_row_number();
	tx_ = Texture::Create(dev_, txFileList_[curIndex]);
	if (signalEnable_)
		signalTextureChanged_(tx_);
}



} // namespace graphics
} // namespace latero


