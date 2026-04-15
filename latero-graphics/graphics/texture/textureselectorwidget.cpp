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

	modeCombo_.set_vexpand();

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

	auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::SAVE);

	std::string dir = std::filesystem::current_path().string();

	auto filter = Gtk::FileFilter::create();
	filter->add_pattern("*.tx");

	dialog->set_current_folder(Gio::File::create_for_path(dir));
	dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("Save", Gtk::ResponseType::OK);
	dialog->set_default_response(Gtk::ResponseType::CANCEL);
	std::string file = texture_->GetXMLFile();
	if (file=="") file = "texture.tx";
	dialog->set_current_name(file);
	dialog->add_filter(filter);

	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		dialog->set_transient_for(*win);
	dialog->signal_response().connect([this, dialog](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
			texture_->Save(dialog->get_file()->get_path());
		delete dialog;
	});
	dialog->show();
}

void TextureSelectorCtrl::OnLoad()
{
	auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::OPEN);

	std::string dir = std::filesystem::current_path().string();

	auto filter = Gtk::FileFilter::create();
	filter->add_pattern("*.tx");

	dialog->set_current_folder(Gio::File::create_for_path(dir));
	dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("Open", Gtk::ResponseType::OK);
	dialog->set_default_response(Gtk::ResponseType::CANCEL);
	dialog->add_filter(filter);
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		dialog->set_transient_for(*win);
	dialog->signal_response().connect([this, dialog](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
		{
			texture_ = Texture::Create(texture_->Dev(), dialog->get_file()->get_path().c_str());
			modeCombo_.SetActive(texture_);
			SignalTextureChanged_();
		}
		delete dialog;
	});
	dialog->show();
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
		//delete widget_; // GTKMM4: fixed seg fault - causing memory leak?
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
		widget_->set_hexpand();
	}
}


void TextureSelectorWidget::OnChanged()
{
	Rebuild();
	SignalTextureChanged_();
}

} // namespace graphics
} // namespace latero
