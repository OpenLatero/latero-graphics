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

#include "patterntexturewidget.h"
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "patterntexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../../graphics/patternpreview.h"
#include "../../graphics/patterncreatordialog.h"


namespace latero {
namespace graphics { 

class PatternTextureGridWidthCtrl : public Gtk::HBox
{
public:
	PatternTextureGridWidthCtrl(PatternTexturePtr peer) :
		adj_(peer->GetGridWidth(),0.1,500), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("grid width", adj_, 3, units::mm)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PatternTextureGridWidthCtrl::OnChanged));
	}
	virtual ~PatternTextureGridWidthCtrl() {};
protected:
	void OnChanged()  { peer_->SetGridWidth(adj_.get_value()); };
	Gtk::Adjustment adj_;
	PatternTexturePtr peer_;
};

class PatternTextureGridHeightCtrl : public Gtk::HBox
{
public:
	PatternTextureGridHeightCtrl(PatternTexturePtr peer) :
		adj_(peer->GetGridHeight(),0.1,500), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("grid height", adj_, 3, units::mm)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PatternTextureGridHeightCtrl::OnChanged));
	}
	virtual ~PatternTextureGridHeightCtrl() {};
protected:
	void OnChanged()  { peer_->SetGridHeight(adj_.get_value()); };
	Gtk::Adjustment adj_;
	PatternTexturePtr peer_;
};


PatternTextureWidget::PatternTextureWidget(PatternTexturePtr peer) :
	peer_(peer)
{
	patternHolder_.set_shadow_type(Gtk::SHADOW_NONE);
	PatternPtr pattern = peer->GetPattern();
	Gtk::Button *loadButton = manage(new Gtk::Button(Gtk::Stock::OPEN));

	Gtk::Box *topbox = manage(new Gtk::HBox);
	topbox->pack_start(*manage(new PatternTextureGridWidthCtrl(peer)));
	topbox->pack_start(*manage(new PatternTextureGridHeightCtrl(peer)));
	topbox->pack_start(*manage(new TextureAdvancedButton(peer)), Gtk::PACK_SHRINK);
	topbox->pack_start(*loadButton, Gtk::PACK_SHRINK);

	Gtk::Box *mainbox = manage(new Gtk::VBox);
	mainbox->pack_start(*topbox, Gtk::PACK_SHRINK);
	mainbox->pack_start(patternHolder_);
	patternHolder_.add(*manage(pattern->CreateWidget(pattern)));
	
	Gtk::VBox *sidebox = manage(new Gtk::VBox);
	sidebox->pack_start(*manage(new TextureInvertCtrl(peer)), Gtk::PACK_SHRINK);
	sidebox->pack_start(*manage(new TextureAmplitudeCtrl(peer)));
	pack_start(*sidebox, Gtk::PACK_SHRINK);
	pack_start(*mainbox, Gtk::PACK_SHRINK);
	pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);

	loadButton->signal_clicked().connect(sigc::mem_fun(*this, &PatternTextureWidget::OnLoad));
}

void PatternTextureWidget::OnLoad()
{
	PatternCreatorDialog dlg(peer_->Dev());
	if (Gtk::RESPONSE_OK == dlg.run())
	{
		PatternPtr newPattern = dlg.CreatePattern();
		if (newPattern)
		{
			Gtk::Widget *wp = patternHolder_.get_child();
			patternHolder_.remove();
			delete wp;

			peer_->SetPattern(newPattern);
			patternHolder_.add(*manage(newPattern->CreateWidget(newPattern)));
			show_all_children();
		}
	}
}







PatternTextureAdvancedWidget::PatternTextureAdvancedWidget(PatternTexturePtr peer) :
	peer_(peer)
{
	patternHolder_.set_shadow_type(Gtk::SHADOW_NONE);
	PatternPtr pattern = peer->GetPattern();
	Gtk::Button *loadButton = manage(new Gtk::Button(Gtk::Stock::OPEN));

	Gtk::Box *topbox = manage(new Gtk::HBox);
	topbox->pack_start(*manage(new PatternTextureGridWidthCtrl(peer)));
	topbox->pack_start(*manage(new PatternTextureGridHeightCtrl(peer)));
	topbox->pack_start(*loadButton, Gtk::PACK_SHRINK);

	Gtk::Box *mainbox = manage(new Gtk::VBox);
	mainbox->pack_start(*topbox, Gtk::PACK_SHRINK);
	mainbox->pack_start(*manage(new TextureMotionCtrl(peer)));
	mainbox->pack_start(patternHolder_);
	patternHolder_.add(*manage(pattern->CreateWidget(pattern)));
	
	Gtk::VBox *sidebox = manage(new Gtk::VBox);
	sidebox->pack_start(*manage(new TextureInvertCtrl(peer)), Gtk::PACK_SHRINK);
	sidebox->pack_start(*manage(new TextureAmplitudeCtrl(peer)));
	pack_start(*sidebox, Gtk::PACK_SHRINK);
	pack_start(*mainbox, Gtk::PACK_SHRINK);
	pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);

	loadButton->signal_clicked().connect(sigc::mem_fun(*this, &PatternTextureAdvancedWidget::OnLoad));
}

void PatternTextureAdvancedWidget::OnLoad()
{
	PatternCreatorDialog dlg(peer_->Dev());
	if (Gtk::RESPONSE_OK == dlg.run())
	{
		PatternPtr newPattern = dlg.CreatePattern();
		if (newPattern)
		{
			Gtk::Widget *wp = patternHolder_.get_child();
			patternHolder_.remove();
			delete wp;

			peer_->SetPattern(newPattern);
			patternHolder_.add(*manage(newPattern->CreateWidget(newPattern)));
			show_all_children();
		}
	}
}

} // namespace graphics
} // namespace latero
