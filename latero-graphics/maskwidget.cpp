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
#include "maskwidget.h"
#include "mask.h"
#include "generator.h"
#include "gtk/numwidget.h"
#include "pointwidget.h"
#include "virtualsurfacewidget.h"
#include <glibmm/main.h>

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

		auto pOpenButton = Gtk::manage(new Gtk::Button());
		pOpenButton->set_icon_name("document-open");

		auto pReloadButton = Gtk::manage(new Gtk::Button());
		pReloadButton->set_icon_name("view-refresh");

		auto *box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		box->append(fileEntry_);
		fileEntry_.set_hexpand();
		box->append(*pOpenButton);
		box->append(*pReloadButton);
		box->append(*Gtk::manage(new MaskSrcTypeCheck(peer)));
		set_child(*box);
 
		pOpenButton->signal_clicked().connect(sigc::mem_fun(*this, &MaskImageWidget::OnOpen));
		pReloadButton->signal_clicked().connect(sigc::mem_fun(*this, &MaskImageWidget::OnReload));
	}
	virtual ~MaskImageWidget() {};

protected:
	void OnOpen()
	{
		auto dialog = new Gtk::FileChooserDialog("Please select an image file.", Gtk::FileChooser::Action::OPEN);

		std::string dir;
		std::string currentFile = peer_->GetImgFile();
		if (currentFile == "")
		{
			dir = std::filesystem::current_path().string();
			dir += "/cards/masks"; // TODO
		}
		else
		{
			dir = currentFile.substr(0, currentFile.find_last_of('/'));
		}

		auto filter = Gtk::FileFilter::create();
		filter->set_name("supported image formats");
		filter->add_pixbuf_formats();

		dialog->set_current_folder(Gio::File::create_for_path(dir));
		dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
		dialog->add_button("Open", Gtk::ResponseType::OK);
		dialog->set_default_response(Gtk::ResponseType::OK);
		dialog->add_filter(filter);

		dialog->signal_response().connect([this, dialog](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
			{
				std::string filename = dialog->get_file()->get_path();
				fileEntry_.set_text(filename.c_str());
				peer_->SetImage(filename, false); // TODO
			}
			delete dialog;
		});
		dialog->show();
	}

	void OnReload()
	{
		peer_->ReloadImage();
	}

private:
	MaskPtr peer_;
	Gtk::Entry fileEntry_;
};


class MaskSizeCtrl : public Gtk::Box, public MaskWidgetCtrl
{
public:
	MaskSizeCtrl(MaskPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), MaskWidgetCtrl(peer),
		wRelAdj_(Gtk::Adjustment::create(100*peer->GetWidth(units::percent),1,200)),
		hRelAdj_(Gtk::Adjustment::create(100*peer->GetHeight(units::percent),1,200)),
		wAbsAdj_(Gtk::Adjustment::create(peer->GetWidth(units::mm),1,2000)),
		hAbsAdj_(Gtk::Adjustment::create(peer->GetHeight(units::mm),1,2000))
	{
		gtk::HNumWidget *wWidget = Gtk::manage(new gtk::HNumWidget("width",wRelAdj_,0, units::percent));
		wWidget->AddUnits(units::mm, wAbsAdj_, 0);
		wWidget->SelectUnits(peer->GetWidthUnits());
		append(*wWidget);
		wRelAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnRelWidthChanged));
		wAbsAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnAbsWidthChanged));
		wWidget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnWidthUnitsChanged));

		gtk::HNumWidget *hWidget = Gtk::manage(new gtk::HNumWidget("height",hRelAdj_,0, units::percent));
		hWidget->AddUnits(units::mm, hAbsAdj_, 0);
		hWidget->SelectUnits(peer->GetHeightUnits());
		append(*hWidget);
		hRelAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnRelHeightChanged));
		hAbsAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnAbsHeightChanged));
		hWidget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnHeightUnitsChanged));
	}

	virtual ~MaskSizeCtrl() {}

	virtual void SynchFromPeer_()
	{
		wRelAdj_->set_value(100*peer_->GetWidth(units::percent));
		hRelAdj_->set_value(100*peer_->GetHeight(units::percent));
		wAbsAdj_->set_value(peer_->GetWidth(units::mm));
		hAbsAdj_->set_value(peer_->GetHeight(units::mm));
	}

	sigc::signal<void()> SignalChanged() { return signalChanged_; };

protected:
	sigc::signal<void()> signalChanged_;

	void OnWidthUnitsChanged(std::string units) { peer_->SetWidthUnits(units); }
	void OnHeightUnitsChanged(std::string units) { peer_->SetHeightUnits(units); }

	void OnRelWidthChanged()
	{
		if (disableSignals_) return;
		peer_->SetWidth(wRelAdj_->get_value()/100, units::percent);
		signalChanged_();
		SynchFromPeer();
	}

	void OnAbsWidthChanged()
	{
		if (disableSignals_) return;
		peer_->SetWidth(wAbsAdj_->get_value(), units::mm);
		signalChanged_();
		SynchFromPeer();
	}

	void OnRelHeightChanged()
	{
		if (disableSignals_) return;
		peer_->SetHeight(hRelAdj_->get_value()/100, units::percent);
		signalChanged_();
		SynchFromPeer();
	}

	void OnAbsHeightChanged()
	{
		if (disableSignals_) return;
		peer_->SetHeight(hAbsAdj_->get_value(), units::mm);
		signalChanged_();
		SynchFromPeer();
	}

    Glib::RefPtr<Gtk::Adjustment> wRelAdj_, hRelAdj_, wAbsAdj_, hAbsAdj_;
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
		centeredRadio_.set_group(freeRadio_);

		centeredRadio_.signal_toggled().connect(sigc::mem_fun(*this, &MaskPositionCtrl::OnModeChanged));
		
		auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		box->append(centeredRadio_);
		box->append(freeRadio_);
		box->append(fileEntry_);
		fileEntry_->set_hexpand();
		set_child(*box);

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
    
	Gtk::CheckButton centeredRadio_, freeRadio_;
	PointWidget posWidget_;
};



class MaskDefaultCtrl : public Gtk::Box, public MaskWidgetCtrl 
{
public:
	MaskDefaultCtrl(MaskPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), MaskWidgetCtrl(peer),
		adj_(Gtk::Adjustment::create(100*peer->GetDefaultAlpha(),0,100))
	{
		append(*Gtk::manage(new Gtk::Label("Default ")));
		append(*Gtk::manage(new Gtk::SpinButton(adj_)));
		append(*Gtk::manage(new Gtk::Label("%")));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskDefaultCtrl::OnChanged));
	}
	virtual ~MaskDefaultCtrl() {}
protected:
	void OnChanged() { peer_->SetDefaultAlpha(adj_->get_value()/100); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

MaskWidget::MaskWidget(MaskPtr peer, const latero::Tactograph *dev) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	auto checkbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));	
	auto maskImageWidget = Gtk::manage(new MaskImageWidget(peer));
	auto maskBlurCheck = Gtk::manage(new MaskBlurCheck(peer));
	auto maskLockAspectRatioCheck = Gtk::manage(new MaskLockAspectRatioCheck(peer));
	auto refMaxWidget = Gtk::manage(new MaskRefMaximizedCheck(peer));
	auto maskDefaultCtrl = Gtk::manage(new MaskDefaultCtrl(peer));
	auto maskSurfaceWidget = Gtk::manage(new MaskSurfaceWidget(dev,peer));

	MaskSizeCtrl *sizeWidget = Gtk::manage(new MaskSizeCtrl(peer));
	ctrls_.push_back(sizeWidget);

	MaskPositionCtrl *posWidget = Gtk::manage(new MaskPositionCtrl(peer));
	ctrls_.push_back(posWidget);

	maskImageWidget->set_vexpand();
	posWidget->set_vexpand();
	sizeWidget->set_vexpand();
	checkbox->set_vexpand();
	maskBlurCheck->set_hexpand();
	maskLockAspectRatioCheck->set_hexpand();
	refMaxWidget->set_hexpand();
	maskDefaultCtrl->set_hexpand();
	box->set_hexpand();
	maskSurfaceWidget->set_hexpand();

	box->append(*maskImageWidget);
	box->append(*posWidget);
	box->append(*sizeWidget);
	box->append(*checkbox);

	checkbox->append(*maskBlurCheck);
	checkbox->append(*maskLockAspectRatioCheck);
	checkbox->append(*refMaxWidget);
	checkbox->append(*maskDefaultCtrl);

	append(*box);
	append(*maskSurfaceWidget);	

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
