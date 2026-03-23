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
#include "texturewidget.h"
#include "texture.h"
#include "../../graphics/patternpreview.h"
#include "../../gtk/numwidget.h"
#include "texture.h"

namespace latero {
namespace graphics { 

CreateTextureDlg::CreateTextureDlg(const latero::Tactograph *dev) : 
	Gtk::Dialog("Create Texture"), txCombo_(dev), dev_(dev)
{
	combo_.append("load from file");
	combo_.append("texture");
	combo_.set_active_text("texture");
	txCombo_.set_sensitive(false);

	combo_.set_vexpand();
	txCombo_.set_vexpand();

	get_content_area()->append(combo_);
	get_content_area()->append(txCombo_);

	combo_.signal_changed().connect( sigc::mem_fun(*this, &CreateTextureDlg::OnComboChanged) );
	
	add_button("OK", Gtk::ResponseType::OK);
	add_button("Cancel", Gtk::ResponseType::CANCEL);	
}


void CreateTextureDlg::OnComboChanged()
{
	txCombo_.set_sensitive(combo_.get_active_text() == "texture");

	// GTKMM4: show file dialog immediately on selection so the path is ready when OK is clicked
	if (combo_.get_active_text() == "load from file")
	{
		loadedFile_.clear();
		auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::OPEN);
		auto filter = Gtk::FileFilter::create();
		filter->add_pattern("*.tx");
		dialog->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
		dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
		dialog->add_button("Open", Gtk::ResponseType::OK);
		dialog->set_default_response(Gtk::ResponseType::CANCEL);
		dialog->add_filter(filter);
		dialog->set_transient_for(*this);
		dialog->signal_response().connect([this, dialog](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
				loadedFile_ = dialog->get_file()->get_path();
			delete dialog;
		});
		dialog->show();
	}
}

TexturePtr CreateTextureDlg::CreateTexture()
{
	std::string type = combo_.get_active_text();
	if (type == "texture")	return txCombo_.GetTexture();
	else if (type == "load from file")
	{
		// GTKMM4: file was already selected in OnComboChanged(); path stored in loadedFile_
		if (!loadedFile_.empty())
			return Texture::Create(dev_, loadedFile_);
		return TexturePtr();
	}
	else return TexturePtr();
}


TextureTDCentricCheck::TextureTDCentricCheck(TexturePtr peer) :
	Gtk::CheckButton("motion independent"), peer_(peer)
{
	set_active(peer->GetTDCentric());
	signal_toggled().connect([this]{ peer_->SetTDCentric(get_active()); });
}


TextureTDCentricCtrl::TextureTDCentricCtrl(TexturePtr peer) : 
	Gtk::Frame("motion independent"), 
	point_(peer->GetTDCentricPos(), 0, peer->Dev()->GetSurfaceWidth(), 0, peer->Dev()->GetSurfaceHeight()),
    peer_(peer)
{
	TextureTDCentricCheck *check = Gtk::manage(new TextureTDCentricCheck(peer));
	check->set_label("");
	
	// TODO: use Check above instead...
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	set_child(*box);
	box->append(*check);
	box->append(point_);
	point_.set_hexpand();
	point_.set_sensitive(check->get_active());
	check->signal_toggled().connect(sigc::mem_fun(*this, &TextureTDCentricCtrl::OnClick));
	point_.SignalValueChanged().connect(sigc::mem_fun(*this, &TextureTDCentricCtrl::OnPosChanged));
}
void TextureTDCentricCtrl::OnClick() { point_.set_sensitive(peer_->GetTDCentric()); };
void TextureTDCentricCtrl::OnPosChanged() { peer_->SetTDCentricPos(point_.GetValue()); };

TextureAmplitudeCtrl::TextureAmplitudeCtrl(TexturePtr peer) :
	adj_(Gtk::Adjustment::create(peer->GetAmplitude()*100,0,100)), peer_(peer) // GTKMM4: missing Box orientation?
{
	auto widget = Gtk::manage(new gtk::VNumWidget(adj_,0, units::percent));
	append(*widget);
	widget->set_hexpand();
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &TextureAmplitudeCtrl::OnChanged));
}
void TextureAmplitudeCtrl::OnChanged() { peer_->SetAmplitude(adj_->get_value()/100); }

TextureInvertCtrl::TextureInvertCtrl(TexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	check_("invert"),
	peer_(peer)
{
	append(check_);
	check_.set_active(peer_->GetInvert());
	check_.signal_toggled().connect(sigc::mem_fun(*this, &TextureInvertCtrl::OnClick));
}
void TextureInvertCtrl::OnClick() { peer_->SetInvert(check_.get_active()); };



class TextureMotionDirectionCtrl : public Gtk::Box
{
public:
	TextureMotionDirectionCtrl(TexturePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetMotionDirection(),0,360)), peer_(peer)
	{
		auto widget = Gtk::manage(new gtk::HNumWidget(adj_,0, units::degree));
		append(*widget);
		widget->set_vexpand();
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &TextureMotionDirectionCtrl::OnChanged));
	}

	virtual ~TextureMotionDirectionCtrl() {};

protected:
	void OnChanged() { peer_->SetMotionDirection(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	TexturePtr peer_;
};

class TextureMotionVelocityCtrl : public Gtk::Box
{
public:
	TextureMotionVelocityCtrl(TexturePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetMotionVelocity(),0,100)), peer_(peer)
	{
		auto widget = Gtk::manage(new gtk::HNumWidget(adj_,1, units::mm_per_sec));
		append(*widget);
		widget->set_vexpand();
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &TextureMotionVelocityCtrl::OnChanged));
	}

	virtual ~TextureMotionVelocityCtrl() {};

protected:
	void OnChanged() { peer_->SetMotionVelocity(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	TexturePtr peer_;
};

TextureMotionCtrl::TextureMotionCtrl(TexturePtr peer) :
	gtk::CheckFrame(peer->GetMotionEnable(), "motion"), peer_(peer)
{
	auto dirCtrl = Gtk::manage(new TextureMotionDirectionCtrl(peer));
	auto velCtrl = Gtk::manage(new TextureMotionVelocityCtrl(peer));

	GetBox().append(*dirCtrl);
	GetBox().append(*velCtrl);

	dirCtrl->set_hexpand();
	velCtrl->set_hexpand();

	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &TextureMotionCtrl::OnClick));
}
void TextureMotionCtrl::OnClick() { peer_->SetMotionEnable(GetCheck().get_active()); }



TextureAdvancedButton::TextureAdvancedButton(TexturePtr peer) :
	Gtk::Button("advanced"),
	adv_(NULL),
	dlg_("advanced texture settings"), peer_(peer)
{
	dlg_.signal_response().connect(sigc::mem_fun(*this, &TextureAdvancedButton::OnResponse));
}

void TextureAdvancedButton::on_clicked()
{ 
	if (adv_)
	{
		dlg_.get_content_area()->remove(*adv_);
		delete adv_;
	}
	adv_ = Gtk::manage(peer_->CreateAdvancedWidget(peer_));
	dlg_.get_content_area()->append(*adv_);
	adv_->set_vexpand();
	dlg_.show();  
	Gtk::Button::on_clicked(); 
}


TextureWidget::TextureWidget(TexturePtr peer) : 
	peer_(peer)
{
	printf("TextureWidget::TextureWidget\n");

}

TextureWidget::~TextureWidget()
{
	printf("TextureWidget::~TextureWidget\n");
}


Gtk::Widget *TextureWidget::CreateLeftPanel()
{
	auto lbox = new Gtk::Box(Gtk::Orientation::VERTICAL);
	
	auto invertCtrl = Gtk::manage(new TextureInvertCtrl(peer_));
	auto ampCtrl = Gtk::manage(new TextureAmplitudeCtrl(peer_));

	lbox->append(*invertCtrl);
	lbox->append(*ampCtrl);

	ampCtrl->set_vexpand();

	return lbox;
}

void TextureWidget::OnAdvanced()
{
	set_sensitive(false);
}

void TextureWidget::OnAdvancedClosed()
{
	set_sensitive(true);
	Rebuild();
}

void TextureWidget::SetContent(Gtk::Widget *widget, bool showPanel, bool showPreview, bool showAdvanced)
{
	unset_child();

	TextureAdvancedButton *advancedButton = Gtk::manage(new TextureAdvancedButton(peer_));
	advancedButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureWidget::OnAdvanced));
	advancedButton->SignalClosed().connect(sigc::mem_fun(*this, &TextureWidget::OnAdvancedClosed));

	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	set_child(*box);

	auto vbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));

	widget->set_vexpand();
	widget->set_valign(Gtk::Align::FILL);

	if (showPanel){
		auto leftPanel = Gtk::manage(CreateLeftPanel());
		box->append(*leftPanel);
		leftPanel->set_hexpand(false);
	}
	box->append(*vbox);
	vbox->set_hexpand();
	vbox->append(*widget);
	if (showAdvanced)
		vbox->append(*advancedButton);
	if (showPreview) 
	{
		auto preview = Gtk::manage(new PatternPreview(peer_));
		box->append(*preview);
		preview->set_hexpand();
	}
}


TextureTDCentricCheck* TextureWidget::CreateTDCentricCheck()
{ return Gtk::manage(new TextureTDCentricCheck(peer_)); }

OscillatorEnableCheck* TextureWidget::CreateVibCheck()
{ return Gtk::manage(new OscillatorEnableCheck(peer_->GetOscillator())); }

TextureMotionCtrl* TextureWidget::CreateMotionWidget()
{ return Gtk::manage(new TextureMotionCtrl(peer_)); }

} // namespace graphics
} // namespace latero
