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
#include <gtkmm/box.h>

namespace latero {
namespace graphics { 

void TextureCombo::Append(std::string txfile)
{
	TexturePtr tx = Texture::Create(dev_,txfile);
	Gtk::TreeModel::Row row = *(model_->append());
	row[columns_.txfile] = txfile;
	row[columns_.imgfile] = tx->GetIconFile();

	Glib::RefPtr<Gdk::Pixbuf> img  = Gdk::Pixbuf::create_from_file(row[columns_.imgfile]);
	if ((img->get_width() != 50) || (img->get_height() != 50))
		img = img->scale_simple(50,50,Gdk::INTERP_BILINEAR);
	row[columns_.img] = img;
}

void TextureCombo::SetActive(TexturePtr tx)
{
	if (!tx) return;
	tx_ = tx;
	std::string txfile = tx->GetXMLFile();

	// check if the txfile is already in the tree
	//bool found = false;
	for (Gtk::TreeModel::Children::iterator iter = model_->children().begin(); iter != model_->children().end(); iter++)
	{
		std::string curfile = (*iter)[columns_.txfile];
		if (curfile == txfile)
		{
			signalEnable_ = false;
			set_active(iter); //  don't reload!
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
			set_active(iter); //  don't reload!
			signalEnable_ = true;
			return;
		}
	}

	// Last resort... add a new icon
	if (txfile=="")
	{
 		// this texture was not loaded from a file, let's create a temporary file
		char file[255];
		sprintf(file,".tmp-%d.tx", rand()%1024);
		tx->Save(file); // TODO: make sure we don't overwrite an existing file
	}
	Append(tx->GetXMLFile());
	SetActive(tx);
}



void TextureCombo::OnComboChanged()
{
	Gtk::TreeModel::iterator iter = get_active();
	if (!iter) return;

	Gtk::TreeModel::Row row = *iter;
	if (!row) return;

	tx_ = Texture::Create(dev_,row[columns_.txfile]);
	if (signalEnable_)
		signalTextureChanged_(tx_);
}

TextureCombo::TextureCombo(TexturePtr tx, std::vector<std::string> textures) :
	signalEnable_(true), textures_(textures), dev_(tx->Dev())
{
	if (textures_.size()==0) textures_ = GetStockTextures();

	// display as a table with 4 columns
	set_wrap_width(4);

	model_ = Gtk::ListStore::create(columns_);
	set_model(model_);
	pack_start(columns_.img);

	for (unsigned int i=0; i<textures_.size(); ++i)
		Append(textures_[i]);

	SetActive(tx);

	signal_changed().connect( sigc::mem_fun(*this, &TextureCombo::OnComboChanged) );
}

TextureCombo::TextureCombo(const latero::Tactograph *dev, std::vector<std::string> textures) :
	signalEnable_(true), textures_(textures), dev_(dev)
{
	if (textures_.size()==0) textures_ = GetStockTextures();

	// display as a table with 4 columns
	set_wrap_width(4);

	model_ = Gtk::ListStore::create(columns_);
	set_model(model_);
	pack_start(columns_.img);

	for (unsigned int i=0; i<textures_.size(); ++i)
		Append(textures_[i]);

	SetActive(Texture::Create(dev,tx_grating_vertical));

	signal_changed().connect( sigc::mem_fun(*this, &TextureCombo::OnComboChanged) );
}

} // namespace graphics
} // namespace latero


