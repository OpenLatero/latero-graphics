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

#include "textureselectorwidget.h"
#include <gtkmm/stock.h>
#include "texture.h"
#include "../../graphics/patternpreview.h"
#include "../../gtk/numwidget.h"
#include <gtkmm/filechooserdialog.h>

namespace latero {
namespace graphics { 

TextureSelectorCtrl::TextureSelectorCtrl(TexturePtr texture) :
	modeCombo_(texture), texture_(texture)
{
	Gtk::Button *saveButton = manage(new Gtk::Button(Gtk::Stock::SAVE));
	Gtk::Button *loadButton = manage(new Gtk::Button(Gtk::Stock::OPEN));

	pack_start(*saveButton, Gtk::PACK_SHRINK);
	pack_start(*loadButton, Gtk::PACK_SHRINK);
	pack_start(modeCombo_);

	saveButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureSelectorCtrl::OnSave));
	loadButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureSelectorCtrl::OnLoad));
	modeCombo_.SignalTextureChanged().connect(sigc::mem_fun(*this, &TextureSelectorCtrl::OnModeChanged));
}

void TextureSelectorCtrl::OnModeChanged(TexturePtr tx)
{
	SetTexture(tx);
}

void TextureSelectorCtrl::OnSave()
{
	if (!texture_) return;
	
	Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_SAVE);

	std::string dir = Glib::get_current_dir();
 
	Gtk::FileFilter filter;
	filter.add_pattern("*.tx");

	dialog.set_current_folder(dir);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	dialog.set_default_response(Gtk::RESPONSE_CANCEL);
	std::string file = texture_->GetXMLFile();
	if (file=="") file = "texture.tx";
	dialog.set_current_name(file);
	dialog.add_filter(filter);

	if (Gtk::RESPONSE_OK == dialog.run())
		 texture_->Save(dialog.get_filename());
}

void TextureSelectorCtrl::OnLoad()
{
	Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_OPEN);

	std::string dir = Glib::get_current_dir();
 
	Gtk::FileFilter filter;
	filter.add_pattern("*.tx");

	dialog.set_current_folder(dir);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	dialog.set_default_response(Gtk::RESPONSE_CANCEL);
	dialog.add_filter(filter);
	if (Gtk::RESPONSE_OK == dialog.run())
	{
		texture_ = Texture::Create(texture_->Dev(), dialog.get_filename().c_str());
		modeCombo_.SetActive(texture_);
		SignalTextureChanged_();
	}
}






TextureSelectorWidget::TextureSelectorWidget(TexturePtr texture) :
	ctrl_(texture),	widget_(NULL)

{
	ctrl_.SignalTextureChanged().connect(sigc::mem_fun(*this, &TextureSelectorWidget::OnChanged));
	Build();
}

TextureSelectorWidget::~TextureSelectorWidget()
{
}

void TextureSelectorWidget::Rebuild()
{
	remove(ctrl_);
	if (widget_)
	{
		remove(*widget_);
		delete widget_;
	}
	Build();
}

void TextureSelectorWidget::Build()
{
	pack_start(ctrl_, Gtk::PACK_SHRINK);
	TexturePtr tx = ctrl_.GetTexture();
	if (tx)
	{
		widget_ = manage(tx->CreateWidget(tx));
		pack_start(*widget_);
	}
	show_all_children();
}


void TextureSelectorWidget::OnChanged()
{
	Rebuild();
	SignalTextureChanged_();
}

} // namespace graphics
} // namespace latero
