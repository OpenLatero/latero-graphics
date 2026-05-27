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

namespace latero::graphics {

TextureSelectorCtrl::TextureSelectorCtrl(TexturePtr texture) :
	Gtk::Box(Gtk::Orientation::VERTICAL), modeDropDown_(texture), texture_(texture)
{
	auto saveButton = Gtk::make_managed<Gtk::Button>();
	saveButton->set_icon_name("document-save");

	auto loadButton = Gtk::make_managed<Gtk::Button>();
	loadButton->set_icon_name("document-open");

	append(*saveButton);
	append(*loadButton);
	append(modeDropDown_);

	modeDropDown_.set_vexpand();

	saveButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureSelectorCtrl::OnSave));
	loadButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureSelectorCtrl::OnLoad));
	modeDropDown_.SignalTextureChanged().connect(sigc::mem_fun(*this, &TextureSelectorCtrl::OnModeChanged));
}

void TextureSelectorCtrl::OnModeChanged(TexturePtr tx)
{
	SetTexture(tx);
}

void TextureSelectorCtrl::OnSave()
{
	// GTKMM4: this currently saves the path along with the filename

	if (!texture_) return;

	auto filter = Gtk::FileFilter::create();
	filter->set_name("texture files");
	filter->add_suffix("tx");
	auto filters = Gio::ListStore<Gtk::FileFilter>::create();
	filters->append(filter);

	std::string file = texture_->GetXMLFile();
	if (file == "") file = "texture.tx";
	file = std::filesystem::path(file).stem().string();

	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select a file...");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_initial_name(file);
	dialog->set_filters(filters);
	dialog->set_default_filter(filter);

	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->save(*win, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto f = dialog->save_finish(result);
			texture_->Save(f->get_path());
		} catch (const Gtk::DialogError&) {}
	});
}

void TextureSelectorCtrl::OnLoad()
{
	auto filter = Gtk::FileFilter::create();
	filter->set_name("texture files");
	filter->add_suffix("tx");
	auto filters = Gio::ListStore<Gtk::FileFilter>::create();
	filters->append(filter);

	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select a file...");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_filters(filters);
	dialog->set_default_filter(filter);

	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->open(*win, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto file = dialog->open_finish(result);
			texture_ = Texture::Create(texture_->Dev(), file->get_path().c_str());
			modeDropDown_.SetActive(texture_);
			SignalTextureChanged_();
		} catch (const Gtk::DialogError&) {}
	});
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
		widget_ = nullptr;
		//delete widget_; // TODO: fixed seg fault - causing memory leak?
	}
	Build();
}

void TextureSelectorWidget::Build()
{
	append(ctrl_);
	TexturePtr tx = ctrl_.GetTexture();
	if (tx)
	{
		widget_ = tx->CreateWidget(tx);
		append(*widget_);
		widget_->set_hexpand();
	}
}


void TextureSelectorWidget::OnChanged()
{
	Rebuild();
	SignalTextureChanged_();
}

} // namespace
