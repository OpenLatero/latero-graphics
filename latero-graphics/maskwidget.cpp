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

#include "maskwidget.h"
#include "mask.h"
#include "generator.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/entry.h>
#include <gtkmm/adjustment.h>
#include "gtk/numwidget.h"
#include "pointwidget.h"
#include "virtualsurfacewidget.h"

namespace latero {
namespace graphics { 

class MaskSrcTypeCheck : public Gtk::CheckButton
{
public:
	MaskSrcTypeCheck(MaskPtr peer) : Gtk::CheckButton("alpha"), peer_(peer)
	{
		set_active(peer->GetSrcTypeAlpha());
	}
	virtual ~MaskSrcTypeCheck() {}
protected:
	virtual void on_toggled() { Gtk::CheckButton::on_toggled(); peer_->SetSrcTypeAlpha(get_active()); }
	MaskPtr peer_;
};


class MaskRefMaximizedCheck : public Gtk::CheckButton
{
public:
	MaskRefMaximizedCheck(MaskPtr peer) : Gtk::CheckButton("reference to maximized"), peer_(peer)
	{
		set_active(peer->GetRefMaximized());
	}
	virtual ~MaskRefMaximizedCheck() {}
protected:
	virtual void on_toggled() { Gtk::CheckButton::on_toggled(); peer_->SetRefMaximized(get_active()); }
	MaskPtr peer_;
};


class MaskSurfaceWidget : public latero::graphics::VirtualSurfaceWidget
{
public:
	MaskSurfaceWidget(const latero::Tactograph* dev, MaskPtr peer) :
		latero::graphics::VirtualSurfaceWidget(dev),
		peer_(peer)
	{
		Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &MaskSurfaceWidget::OnCheckPeer),
			(uint)333, Glib::PRIORITY_DEFAULT_IDLE);
	}
	virtual ~MaskSurfaceWidget() {};

protected:
	bool OnCheckPeer()
	{
		if (peer_->GetLastModified() > bgUpdateTime_) RefreshBackground();
		return true;
	}

	virtual void on_size_allocate(Gtk::Allocation& allocation)
	{
		latero::graphics::VirtualSurfaceWidget::on_size_allocate(allocation);
		RefreshBackground();
	}

	void RefreshBackground()
	{
		bgUpdateTime_ = boost::posix_time::microsec_clock::universal_time();
		Set(peer_->GetIllustration(GetWidth(),GetHeight()));
	}

private:
	MaskPtr peer_;
	boost::posix_time::ptime bgUpdateTime_;
};


class MaskBlurCheck : public Gtk::CheckButton
{
public:
	MaskBlurCheck(MaskPtr peer) : Gtk::CheckButton("blur"), peer_(peer)
	{
		set_active(peer->IsBlurred());
	}
	virtual ~MaskBlurCheck() {}
protected:
	virtual void on_toggled() { Gtk::CheckButton::on_toggled(); peer_->SetBlurred(get_active()); }
	MaskPtr peer_;
};


class MaskLockAspectRatioCheck : public Gtk::CheckButton
{
public:
	MaskLockAspectRatioCheck(MaskPtr peer) : Gtk::CheckButton("lock aspect ratio"), peer_(peer)
	{
		set_active(peer->GetLockAspectRatio());
	}
	virtual ~MaskLockAspectRatioCheck() {}
protected:
	virtual void on_toggled() { Gtk::CheckButton::on_toggled(); peer_->SetLockAspectRatio(get_active()); }
	MaskPtr peer_;
};

class MaskImageWidget : public Gtk::Frame
{
public:
	MaskImageWidget(MaskPtr peer) :
		Gtk::Frame("image"), peer_(peer)
	{
		fileEntry_.set_text(peer->GetImgFile());

		Gtk::Button *pOpenButton = manage(new Gtk::Button(Gtk::Stock::OPEN));
		Gtk::Button *pReloadButton = manage(new Gtk::Button(Gtk::Stock::REFRESH));

		Gtk::HBox *box = manage(new Gtk::HBox);
		box->pack_start(fileEntry_);
		box->pack_start(*pOpenButton, Gtk::PACK_SHRINK);
		box->pack_start(*pReloadButton, Gtk::PACK_SHRINK);
		box->pack_start(*manage(new MaskSrcTypeCheck(peer)), Gtk::PACK_SHRINK);
		add(*box);
 
		pOpenButton->signal_released().connect(sigc::mem_fun(*this, &MaskImageWidget::OnOpen));
		pReloadButton->signal_released().connect(sigc::mem_fun(*this, &MaskImageWidget::OnReload));
	}
	virtual ~MaskImageWidget() {};

protected:
	void OnOpen()
	{
		Gtk::FileChooserDialog dialog("Please select an image file.");
 
		std::string dir;
		std::string currentFile = peer_->GetImgFile();
		if (currentFile == "")
		{
			dir = Glib::get_current_dir();
			dir += "/cards/masks"; // TODO
		}
		else
		{
			dir = currentFile.substr(0, currentFile.find_last_of('/'));
		}

		Gtk::FileFilter filter;
		filter.set_name("supported image formats");
		filter.add_pixbuf_formats();
 
		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_OK);
		dialog.add_filter(filter);

		if (Gtk::RESPONSE_OK == dialog.run())		
		{
			std::string filename = dialog.get_filename();
			fileEntry_.set_text(filename.c_str());
			peer_->SetImage(filename,false); // TODO
		}
	}

	void OnReload()
	{
		peer_->ReloadImage();
	}

private:
	MaskPtr peer_;
	Gtk::Entry fileEntry_;
};


class MaskSizeCtrl : public Gtk::HBox, public MaskWidgetCtrl
{
public:
	MaskSizeCtrl(MaskPtr peer) : 
		MaskWidgetCtrl(peer), 
		wRelAdj_(100*peer->GetWidth(units::percent),1,200),
		hRelAdj_(100*peer->GetHeight(units::percent),1,200),
		wAbsAdj_(peer->GetWidth(units::mm),1,2000),
		hAbsAdj_(peer->GetHeight(units::mm),1,2000)
	{
		gtk::HNumWidget *wWidget = manage(new gtk::HNumWidget("width",wRelAdj_,0, units::percent));
		wWidget->AddUnits(units::mm, &wAbsAdj_, 0);
		wWidget->SelectUnits(peer->GetWidthUnits());
		add(*wWidget);
		wRelAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnRelWidthChanged));
		wAbsAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnAbsWidthChanged));
		wWidget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnWidthUnitsChanged));

		gtk::HNumWidget *hWidget = manage(new gtk::HNumWidget("height",hRelAdj_,0, units::percent));
		hWidget->AddUnits(units::mm, &hAbsAdj_, 0);
		hWidget->SelectUnits(peer->GetHeightUnits());
		add(*hWidget);
		hRelAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnRelHeightChanged));
		hAbsAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnAbsHeightChanged));
		hWidget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnHeightUnitsChanged));
	}

	virtual ~MaskSizeCtrl() {}

	virtual void SynchFromPeer_()
	{
		wRelAdj_.set_value(100*peer_->GetWidth(units::percent));
		hRelAdj_.set_value(100*peer_->GetHeight(units::percent));
		wAbsAdj_.set_value(peer_->GetWidth(units::mm));
		hAbsAdj_.set_value(peer_->GetHeight(units::mm));
	}

	sigc::signal<void> SignalChanged() { return signalChanged_; };

protected:
	sigc::signal<void> signalChanged_;

	void OnWidthUnitsChanged(std::string units) { peer_->SetWidthUnits(units); }
	void OnHeightUnitsChanged(std::string units) { peer_->SetHeightUnits(units); }

	void OnRelWidthChanged()
	{
		if (disableSignals_) return;
		peer_->SetWidth(wRelAdj_.get_value()/100, units::percent);
		signalChanged_();
		SynchFromPeer();
	}

	void OnAbsWidthChanged()
	{
		if (disableSignals_) return;
		peer_->SetWidth(wAbsAdj_.get_value(), units::mm);
		signalChanged_();
		SynchFromPeer();
	}

	void OnRelHeightChanged()
	{
		if (disableSignals_) return;
		peer_->SetHeight(hRelAdj_.get_value()/100, units::percent);
		signalChanged_();
		SynchFromPeer();
	}

	void OnAbsHeightChanged()
	{
		if (disableSignals_) return;
		peer_->SetHeight(hAbsAdj_.get_value(), units::mm);
		signalChanged_();
		SynchFromPeer();
	}

	Gtk::Adjustment wRelAdj_, hRelAdj_, wAbsAdj_, hAbsAdj_;
};


class MaskPositionCtrl : public Gtk::Frame, public MaskWidgetCtrl 
{
public:
	MaskPositionCtrl(MaskPtr peer) :
 		Gtk::Frame("position"),   
		MaskWidgetCtrl(peer),  
		centeredRadio_("centered"), 
		posWidget_(peer->GetOffset(),
				-peer->GetSurfaceSize().x*2, peer->GetSurfaceSize().x*2,
				-peer->GetSurfaceSize().y*2, peer->GetSurfaceSize().y*2)
	{
		Gtk::RadioButton::Group group = centeredRadio_.get_group();
		freeRadio_.set_group(group);
		centeredRadio_.signal_clicked().connect(sigc::mem_fun(*this, &MaskPositionCtrl::OnModeChanged));
		
		Gtk::HBox *box = manage(new Gtk::HBox);
		box->pack_start(centeredRadio_, Gtk::PACK_SHRINK);
		box->pack_start(freeRadio_, Gtk::PACK_SHRINK);
		box->pack_start(posWidget_);
		add(*box);

		posWidget_.SignalValueChanged().connect(sigc::mem_fun(*this, &MaskPositionCtrl::OnPosChanged));

		SynchFromPeer();
	}

	virtual ~MaskPositionCtrl() {}

	virtual void SynchFromPeer_()
	{
		posWidget_.SetValue(peer_->GetOffset());
		posWidget_.set_sensitive(!peer_->GetCentered());
		centeredRadio_.set_active(peer_->GetCentered());
		freeRadio_.set_active(!peer_->GetCentered());
	}

protected:

	void OnModeChanged()
	{
		if (disableSignals_) return;
		peer_->SetCentered(centeredRadio_.get_active());
		SynchFromPeer();
	}

	void OnPosChanged()
	{
		if (disableSignals_) return;
		peer_->SetOffset(posWidget_.GetValue());
		SynchFromPeer();
	}
    
	Gtk::RadioButton centeredRadio_, freeRadio_;
	PointWidget posWidget_;
};



class MaskDefaultCtrl : public Gtk::HBox, public MaskWidgetCtrl 
{
public:
	MaskDefaultCtrl(MaskPtr peer) :
		MaskWidgetCtrl(peer), 
		adj_(100*peer->GetDefaultAlpha(),0,100)
	{
		pack_start(*manage(new Gtk::Label("Default ")), Gtk::PACK_SHRINK);
		pack_start(*manage(new Gtk::SpinButton(adj_)), Gtk::PACK_SHRINK);
		pack_start(*manage(new Gtk::Label("%")), Gtk::PACK_SHRINK);
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &MaskDefaultCtrl::OnChanged));
	}
	virtual ~MaskDefaultCtrl() {}
protected:
	void OnChanged() { peer_->SetDefaultAlpha(adj_.get_value()/100); }
	Gtk::Adjustment adj_;
};

MaskWidget::MaskWidget(MaskPtr peer, const latero::Tactograph *dev)
{
	MaskSizeCtrl *sizeWidget = manage(new MaskSizeCtrl(peer));
	ctrls_.push_back(sizeWidget);

	MaskPositionCtrl *posWidget = manage(new MaskPositionCtrl(peer));
	ctrls_.push_back(posWidget);

	Gtk::VBox *box = manage(new Gtk::VBox);
	box->pack_start(*manage(new MaskImageWidget(peer)));
	box->pack_start(*posWidget);
	box->pack_start(*sizeWidget);

	Gtk::HBox *checkbox = manage(new Gtk::HBox);
	checkbox->pack_start(*manage(new MaskBlurCheck(peer)));
	checkbox->pack_start(*manage(new MaskLockAspectRatioCheck(peer)));
	Gtk::CheckButton *refMaxWidget = manage(new MaskRefMaximizedCheck(peer));
	checkbox->pack_start(*refMaxWidget);
	checkbox->pack_start(*manage(new MaskDefaultCtrl(peer)));
	box->pack_start(*checkbox);

	pack_start(*box);	
	pack_start(*manage(new MaskSurfaceWidget(dev,peer)));	

 	refMaxWidget->signal_toggled().connect(sigc::mem_fun(*this, &MaskWidget::SynchFromPeer));
	sizeWidget->SignalChanged().connect(sigc::mem_fun(*this, &MaskWidget::SynchFromPeer));
}

void MaskWidget::SynchFromPeer()
{
	for (unsigned int i=0; i<ctrls_.size(); ++i)
		ctrls_[i]->SynchFromPeer();
}

} // namespace graphics
} // namespace latero
