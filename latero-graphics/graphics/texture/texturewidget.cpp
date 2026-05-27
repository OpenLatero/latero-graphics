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

namespace latero::graphics {

CreateTextureDlg::CreateTextureDlg(const latero::Tactograph *dev) :
	Gtk::Window(), txDropDown_(dev), dev_(dev)
{
	set_title("Create Texture");
	set_modal(true);
	set_default_size(300,-1);

	optionsList_ = Gtk::StringList::create({});
	optionsList_->append("load from file");
	optionsList_->append("texture");
	optionsDropDown_ = Gtk::make_managed<Gtk::DropDown>(optionsList_);
	optionsDropDown_->set_selected(1); // "texture"
	txDropDown_.set_sensitive(true); // as long as texture is the default

	auto okButton     = Gtk::make_managed<Gtk::Button>("OK");
	auto cancelButton = Gtk::make_managed<Gtk::Button>("Cancel");
	auto bbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	bbox->set_halign(Gtk::Align::END);
	bbox->set_spacing(6);
	bbox->append(*okButton);
	bbox->append(*cancelButton);

	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	vbox->set_margin(12);
	vbox->set_spacing(6);
	vbox->append(*optionsDropDown_);
	vbox->append(txDropDown_);
	vbox->append(*bbox);
	set_child(*vbox);

	optionsDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &CreateTextureDlg::OnDropDownChanged));
	okButton->signal_clicked().connect([this]{
		signalResponse_.emit((int)Gtk::ResponseType::OK);
	});
	cancelButton->signal_clicked().connect([this]{
		signalResponse_.emit((int)Gtk::ResponseType::CANCEL);
	});
}


void CreateTextureDlg::OnDropDownChanged()
{
	auto activeText = std::string(optionsList_->get_string(optionsDropDown_->get_selected()));
	txDropDown_.set_sensitive(activeText == "texture");

	if (activeText == "load from file")
	{
		loadedFile_.clear();
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
		dialog->open(*this, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
			try {
				auto file = dialog->open_finish(result);
				loadedFile_ = file->get_path();
			} catch (const Gtk::DialogError&) {}
		});
	}
}

TexturePtr CreateTextureDlg::CreateTexture()
{
	std::string type = std::string(optionsList_->get_string(optionsDropDown_->get_selected()));
	if (type == "texture")	return txDropDown_.GetTexture();
	else if (type == "load from file")
	{
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
	TextureTDCentricCheck *check = Gtk::make_managed<TextureTDCentricCheck>(peer);
	check->set_label("");
	
	// TODO: use Check above instead...
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
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
	adj_(Gtk::Adjustment::create(peer->GetAmplitude()*100,0,100)), peer_(peer)
{
	auto widget = Gtk::make_managed<gtk::VNumWidget>(adj_,0, units::percent);
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
		auto widget = Gtk::make_managed<gtk::HNumWidget>(adj_,0, units::degree);
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
		auto widget = Gtk::make_managed<gtk::HNumWidget>(adj_,1, units::mm_per_sec);
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
	auto dirCtrl = Gtk::make_managed<TextureMotionDirectionCtrl>(peer);
	auto velCtrl = Gtk::make_managed<TextureMotionVelocityCtrl>(peer);

	GetBox().append(*dirCtrl);
	GetBox().append(*velCtrl);

	dirCtrl->set_hexpand();
	velCtrl->set_hexpand();

	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &TextureMotionCtrl::OnClick));
}
void TextureMotionCtrl::OnClick() { peer_->SetMotionEnable(GetCheck().get_active()); }



TextureAdvancedButton::TextureAdvancedButton(TexturePtr peer) :
	Gtk::Button("advanced"),
	adv_(nullptr),
	peer_(peer)
{
	dlg_.set_title("advanced texture settings");
	dlg_.set_modal(true);
	dlg_.set_hide_on_close(true);
	dlg_.signal_hide().connect([this]{ signalClosed_(); });
}

void TextureAdvancedButton::on_clicked()
{
	if (adv_)
	{
		dlg_.unset_child();
		//delete adv_;
	}
	adv_ = peer_->CreateAdvancedWidget(peer_);
	dlg_.set_child(*adv_);
	adv_->set_vexpand();
	if (auto win = dynamic_cast<Gtk::Window*>(get_root()))
		dlg_.set_transient_for(*win);
	dlg_.present();
	Gtk::Button::on_clicked();
}


TextureWidget::TextureWidget(TexturePtr peer) : 
	peer_(peer)
{
}

TextureWidget::~TextureWidget()
{
}


Gtk::Widget *TextureWidget::CreateLeftPanel()
{
	auto lbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	
	auto invertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer_);
	auto ampCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer_);

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

	TextureAdvancedButton *advancedButton = Gtk::make_managed<TextureAdvancedButton>(peer_);
	advancedButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureWidget::OnAdvanced));
	advancedButton->SignalClosed().connect(sigc::mem_fun(*this, &TextureWidget::OnAdvancedClosed));

	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	set_child(*box);

	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);

	widget->set_vexpand();
	widget->set_valign(Gtk::Align::FILL);

	if (showPanel){
		auto leftPanel = CreateLeftPanel();
		box->append(*leftPanel);
		leftPanel->set_hexpand(false);
	}
	box->append(*vbox);
	vbox->set_hexpand();
	vbox->append(*widget);
	if (showAdvanced)
		vbox->append(*advancedButton);
	if (showPreview) 
		box->append(*Gtk::make_managed<PatternPreview>(peer_));
}


TextureTDCentricCheck* TextureWidget::CreateTDCentricCheck()
{ return Gtk::make_managed<TextureTDCentricCheck>(peer_); }

OscillatorEnableCheck* TextureWidget::CreateVibCheck()
{ return Gtk::make_managed<OscillatorEnableCheck>(peer_->GetOscillator()); }

TextureMotionCtrl* TextureWidget::CreateMotionWidget()
{ return Gtk::make_managed<TextureMotionCtrl>(peer_); }

} // namespace
