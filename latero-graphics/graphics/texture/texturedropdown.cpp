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

#include "texturedropdown.h"
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

void TextureDropDown::Init()
{
	auto factory = Gtk::SignalListItemFactory::create();
	factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		item->set_child(*Gtk::make_managed<Gtk::Image>());
	});
	factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& item) {
		auto pos = item->get_position();
		if (pos < pixbufList_.size())
			if (auto image = dynamic_cast<Gtk::Image*>(item->get_child()))
				image->set(pixbufList_[pos]);
	});
	dropDown_.set_factory(factory);
	dropDown_.set_list_factory(factory);
	append(dropDown_);
	dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &TextureDropDown::OnComboChanged));
}

TextureDropDown::TextureDropDown(TexturePtr tx, std::vector<std::string> textures) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	countingList_(Gtk::StringList::create({})),
	dropDown_(countingList_),
	signalEnable_(true), textureList_(textures), dev_(tx->Dev())
{
	if (textureList_.size()==0) 
		textureList_ = GetStockTextures();
	Init();
	for (unsigned int i=0; i<textureList_.size(); ++i)
		Append(textureList_[i]);
	SetActive(tx);
}

TextureDropDown::TextureDropDown(const latero::Tactograph *dev, std::vector<std::string> textures) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	countingList_(Gtk::StringList::create({})),
	dropDown_(countingList_),
	signalEnable_(true), textureList_(textures), dev_(dev)
{
	if (textureList_.size()==0) 
		textureList_ = GetStockTextures();
	Init();
	for (unsigned int i=0; i<textureList_.size(); ++i)
		Append(textureList_[i]);
	SetActive(Texture::Create(dev, tx_grating_vertical));
}

void TextureDropDown::Append(std::string txfile)
{
	TexturePtr tx = Texture::Create(dev_, txfile);
	std::string imgFile = tx->GetIconFile();

	txFileList_.push_back(txfile);
	imgFileList_.push_back(imgFile);

	auto img = Gdk::Pixbuf::create_from_file(imgFile);
	if ((img->get_width() != 50) || (img->get_height() != 50))
		img = img->scale_simple(50, 50, Gdk::InterpType::BILINEAR);
	pixbufList_.push_back(img);
	countingList_->append("");
}

void TextureDropDown::SetActive(TexturePtr tx)
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
			dropDown_.set_selected(i); //  don't reload!
			signalEnable_ = true;
			return;
		}
	}

	// Try to find a matching icon file
	std::string iconfile = tx->GetDefaultIconFile();
	for (unsigned int i=0; i<imgFileList_.size(); ++i)
	{
		std::string imgfile = imgFileList_[i];
		if (imgfile == iconfile)
		{
			signalEnable_ = false;
			dropDown_.set_selected(i); //  don't reload!
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



void TextureDropDown::OnComboChanged()
{
	guint curIndex = dropDown_.get_selected();
	if (curIndex >= txFileList_.size()) return;
	tx_ = Texture::Create(dev_, txFileList_[curIndex]);
	if (signalEnable_)
		signalTextureChanged_(tx_);
}



} // namespace graphics
} // namespace latero


