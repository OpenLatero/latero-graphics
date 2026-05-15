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

#ifndef LATERO_GRAPHICS_PLANAR_TEXTURE_DROPDOWN
#define LATERO_GRAPHICS_PLANAR_TEXTURE_DROPDOWN

#include "texturefwd.h"
#include <gtkmm.h>
#include <latero/tactograph.h>

namespace latero {
namespace graphics {

class TextureDropDown : public Gtk::Box
{
public:
	TextureDropDown(TexturePtr tx, std::vector<std::string> textures = std::vector<std::string>());
	TextureDropDown(const latero::Tactograph *dev, std::vector<std::string> textures = std::vector<std::string>());

	virtual ~TextureDropDown() {};
	void Append(std::string txfile);
	void SetActive(TexturePtr tx);
	sigc::signal<void(TexturePtr)> SignalTextureChanged() { return signalTextureChanged_; }

	TexturePtr GetTexture() { return tx_; }
protected:
	sigc::signal<void(TexturePtr)> signalTextureChanged_;
	void Init();
	void OnDropDownChanged();

    bool signalEnable_;
	TexturePtr tx_;
	
	Glib::RefPtr<Gtk::StringList> countingList_;
	Gtk::DropDown dropDown_;

    const latero::Tactograph *dev_; // tmp?

	// TODO: merge those three items in a single struct/class
	std::vector<std::string> txFileList_;
	std::vector<std::string> imgFileList_;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufList_;
	std::vector<std::string> textureList_;
};

} // namespace graphics
} // namespace latero

#endif

