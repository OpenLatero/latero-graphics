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

#include "texturewidget.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include "texture.h"
#include "../../graphics/patternpreview.h"
#include "../../gtk/numwidget.h"
#include "texture.h"

namespace latero {
namespace graphics { 

CreateTextureDlg::CreateTextureDlg(const latero::Tactograph *dev) : 
	Gtk::Dialog("Create Texture"), txCombo_(dev), dev_(dev)
{
	combo_.append_text("load from file");
	combo_.append_text("texture");
	combo_.set_active_text("texture");
	txCombo_.set_sensitive(false);

	get_vbox()->pack_start(combo_);
	get_vbox()->pack_start(txCombo_);

	combo_.signal_changed().connect( sigc::mem_fun(*this, &CreateTextureDlg::OnComboChanged) );
	
	add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	show_all_children();		
}


void CreateTextureDlg::OnComboChanged()
{
	txCombo_.set_sensitive(combo_.get_active_text() == "texture");
}

TexturePtr CreateTextureDlg::CreateTexture()
{
	std::string type = combo_.get_active_text();
	if (type == "texture")	return txCombo_.GetTexture();
	else if (type == "load from file")
	{
		Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_SAVE);
			
		std::string dir = Glib::get_current_dir();
 
		Gtk::FileFilter filter;
		filter.add_pattern("*.tx");

		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_CANCEL);
		std::string file = "new.tx";
		dialog.set_current_name(file);
		dialog.add_filter(filter);

		if (Gtk::RESPONSE_OK == dialog.run())
			return Texture::Create(dev_,dialog.get_filename()); 
		else
			return TexturePtr();
	}
	else return TexturePtr();
}


TextureTDCentricCheck::TextureTDCentricCheck(TexturePtr peer) : 
	Gtk::CheckButton("motion independent"), peer_(peer)
{
	set_active(peer->GetTDCentric());
}
void TextureTDCentricCheck::on_clicked() { peer_->SetTDCentric(!get_active()); Gtk::CheckButton::on_clicked(); };


TextureTDCentricCtrl::TextureTDCentricCtrl(TexturePtr peer) : 
	Gtk::Frame("motion independent"), 
	point_(peer->GetTDCentricPos(), 0, peer->Dev()->GetSurfaceWidth(), 0, peer->Dev()->GetSurfaceHeight()),
    peer_(peer)
{
	TextureTDCentricCheck *check = manage(new TextureTDCentricCheck(peer));
	check->set_label("");
	
	// TODO: use Check above instead...
	Gtk::HBox *box = manage(new Gtk::HBox);
	add(*box);
	box->pack_start(*check, Gtk::PACK_SHRINK);
	box->pack_start(point_);
	point_.set_sensitive(check->get_active());
	check->signal_clicked().connect(sigc::mem_fun(*this, &TextureTDCentricCtrl::OnClick));
	point_.SignalValueChanged().connect(sigc::mem_fun(*this, &TextureTDCentricCtrl::OnPosChanged));
}
void TextureTDCentricCtrl::OnClick() { point_.set_sensitive(peer_->GetTDCentric()); };
void TextureTDCentricCtrl::OnPosChanged() { peer_->SetTDCentricPos(point_.GetValue()); };

TextureAmplitudeCtrl::TextureAmplitudeCtrl(TexturePtr peer) :
	adj_(peer->GetAmplitude()*100,0,100), peer_(peer)
{
	pack_start(*manage(new gtk::VNumWidget(adj_,0, units::percent)));
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &TextureAmplitudeCtrl::OnChanged));
}
void TextureAmplitudeCtrl::OnChanged() { peer_->SetAmplitude(adj_.get_value()/100); }

TextureInvertCtrl::TextureInvertCtrl(TexturePtr peer) :
	check_("invert"),
	peer_(peer)
{
	add(check_);
	check_.set_active(peer_->GetInvert());
	check_.signal_clicked().connect(sigc::mem_fun(*this, &TextureInvertCtrl::OnClick));
}
void TextureInvertCtrl::OnClick() { peer_->SetInvert(check_.get_active()); };



class TextureMotionDirectionCtrl : public Gtk::VBox
{
public:
	TextureMotionDirectionCtrl(TexturePtr peer) :
		adj_(peer->GetMotionDirection(),0,360), peer_(peer)
	{
		pack_start(*manage(new gtk::HNumWidget(adj_,0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &TextureMotionDirectionCtrl::OnChanged));
	}

	virtual ~TextureMotionDirectionCtrl() {};

protected:
	void OnChanged() { peer_->SetMotionDirection(adj_.get_value()); };
	Gtk::Adjustment adj_;
	TexturePtr peer_;
};

class TextureMotionVelocityCtrl : public Gtk::VBox
{
public:
	TextureMotionVelocityCtrl(TexturePtr peer) :
		adj_(peer->GetMotionVelocity(),0,100), peer_(peer)
	{
		pack_start(*manage(new gtk::HNumWidget(adj_,1, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &TextureMotionVelocityCtrl::OnChanged));
	}

	virtual ~TextureMotionVelocityCtrl() {};

protected:
	void OnChanged() { peer_->SetMotionVelocity(adj_.get_value()); };
	Gtk::Adjustment adj_;
	TexturePtr peer_;
};

TextureMotionCtrl::TextureMotionCtrl(TexturePtr peer) :
	gtk::CheckFrame(peer->GetMotionEnable(), "motion"), peer_(peer)
{
	GetBox().pack_start(*manage(new TextureMotionDirectionCtrl(peer)));
	GetBox().pack_start(*manage(new TextureMotionVelocityCtrl(peer)));
	GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &TextureMotionCtrl::OnClick));
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
		dlg_.get_vbox()->remove(*adv_);
		delete adv_;
	}
	adv_ = manage(peer_->CreateAdvancedWidget(peer_));
	dlg_.get_vbox()->pack_start(*adv_);
	dlg_.show_all_children();
	dlg_.show();  
	Gtk::Button::on_clicked(); 
}


TextureWidget::TextureWidget(TexturePtr peer) : 
	peer_(peer)
{
	printf("TextureWidget::TextureWidget\n");
	set_shadow_type(Gtk::SHADOW_NONE);
}

TextureWidget::~TextureWidget()
{
	printf("TextureWidget::~TextureWidget\n");
}


Gtk::Widget *TextureWidget::CreateLeftPanel()
{
	Gtk::VBox *lbox = new Gtk::VBox;
	lbox->pack_start(*manage(new TextureInvertCtrl(peer_)), Gtk::PACK_SHRINK);
	lbox->pack_start(*manage(new TextureAmplitudeCtrl(peer_)));
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
	Gtk::Widget *wp = get_child();
	remove();
	delete wp;

	TextureAdvancedButton *advancedButton = manage(new TextureAdvancedButton(peer_));
	advancedButton->signal_clicked().connect(sigc::mem_fun(*this, &TextureWidget::OnAdvanced));
	advancedButton->SignalClosed().connect(sigc::mem_fun(*this, &TextureWidget::OnAdvancedClosed));

	Gtk::HBox *box = manage(new Gtk::HBox);
	add(*box);

	Gtk::VBox *vbox = manage(new Gtk::VBox);

	if (showPanel) box->pack_start(*manage(CreateLeftPanel()), Gtk::PACK_SHRINK);
	box->pack_start(*vbox);
	vbox->pack_start(*widget);
	if (showAdvanced) vbox->pack_start(*advancedButton, Gtk::PACK_SHRINK);
	if (showPreview) box->pack_start(*manage(new PatternPreview(peer_)), Gtk::PACK_SHRINK);

	show_all_children();
}


TextureTDCentricCheck* TextureWidget::CreateTDCentricCheck()
{ return manage(new TextureTDCentricCheck(peer_)); }

OscillatorEnableCheck* TextureWidget::CreateVibCheck()
{ return manage(new OscillatorEnableCheck(peer_->GetOscillator())); }

TextureMotionCtrl* TextureWidget::CreateMotionWidget()
{ return manage(new TextureMotionCtrl(peer_)); }

} // namespace graphics
} // namespace latero
