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

#ifndef LATERO_GRAPHICS_PLANAR_TEXTURE_COMBO
#define LATERO_GRAPHICS_PLANAR_TEXTURE_COMBO

#include "texturefwd.h"
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <latero/tactograph.h>

namespace latero {
namespace graphics { 

class TextureCombo : public Gtk::ComboBox
{
public:
	TextureCombo(TexturePtr tx, std::vector<std::string> textures = std::vector<std::string>());
	TextureCombo(const latero::Tactograph *dev, std::vector<std::string> textures = std::vector<std::string>());

	virtual ~TextureCombo() {};
	void Append(std::string txfile);
	void SetActive(TexturePtr tx);
	sigc::signal<void,TexturePtr> SignalTextureChanged() { return signalTextureChanged_; }

	TexturePtr GetTexture() { return tx_; }
protected:
	sigc::signal<void,TexturePtr> signalTextureChanged_;
	void OnComboChanged();

	class Columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Columns() { add(txfile); add(img); add(imgfile); }
		Gtk::TreeModelColumn<std::string> imgfile;
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > img;
		Gtk::TreeModelColumn<std::string> txfile;
	};

    bool signalEnable_;
	std::vector<std::string> textures_;
	TexturePtr tx_;
	Columns columns_;
	Glib::RefPtr<Gtk::ListStore> model_;
    const latero::Tactograph *dev_; // tmp?
};

} // namespace graphics
} // namespace latero

#endif

