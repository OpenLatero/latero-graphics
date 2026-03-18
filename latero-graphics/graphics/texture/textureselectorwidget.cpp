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

#include <filesystem>
#include "textureselectorwidget.h"
#include "texture.h"
#include "../../graphics/patternpreview.h"
#include "../../gtk/numwidget.h"

namespace latero {
namespace graphics { 

TextureSelectorCtrl::TextureSelectorCtrl(TexturePtr texture) :
	Gtk::Box(Gtk::Orientation::VERTICAL), modeCombo_(texture), texture_(texture)
{
	auto saveButton = Gtk::manage(new Gtk::Button());
	saveButton->set_icon_name("document-save");

	auto loadButton = Gtk::manage(new Gtk::Button());
	loadButton->set_icon_name("document-open");

	append(*saveButton);
	append(*loadButton);
	append(modeCombo_);

	modeCombo_->set_vexpand();

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

	std::string dir = std::filesystem::current_path().string();
 
    Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
	filter->add_pattern("*.tx");

	dialog.set_current_folder(Gio::File::create_for_path(dir));
	dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Save", Gtk::RESPONSE_OK);
	dialog.set_default_response(Gtk::RESPONSE_CANCEL);
	std::string file = texture_->GetXMLFile();
	if (file=="") file = "texture.tx";
	dialog.set_current_name(file);
	dialog.add_filter(filter);

	if (Gtk::RESPONSE_OK == dialog.run())
		 texture_->Save(dialog.get_file()->get_path()); // GTKMM4
}

void TextureSelectorCtrl::OnLoad()
{
	Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_OPEN);

	std::string dir = std::filesystem::current_path().string();
 
    Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
	filter->add_pattern("*.tx");

	dialog.set_current_folder(Gio::File::create_for_path(dir));
	dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Open", Gtk::RESPONSE_OK);
	dialog.set_default_response(Gtk::RESPONSE_CANCEL);
	dialog.add_filter(filter);
	if (Gtk::RESPONSE_OK == dialog.run())
	{
		texture_ = Texture::Create(texture_->Dev(), dialog.get_file()->get_path().c_str());// GTKMM4
		modeCombo_.SetActive(texture_);
		SignalTextureChanged_();
	}
}






TextureSelectorWidget::TextureSelectorWidget(TexturePtr texture) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), ctrl_(texture), widget_(NULL)

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
	append(ctrl_);
	TexturePtr tx = ctrl_.GetTexture();
	if (tx)
	{
		widget_ = Gtk::manage(tx->CreateWidget(tx));
		append(*widget_);
		widget->set_hexpand();
	}
}


void TextureSelectorWidget::OnChanged()
{
	Rebuild();
	SignalTextureChanged_();
}

} // namespace graphics
} // namespace latero
