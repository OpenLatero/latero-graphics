#include <filesystem>
#include "maskwidget.h"
#include "mask.h"
#include "generator.h"
#include "gtk/numwidget.h"
#include "pointwidget.h"
#include "tactographview.h"
#include <glibmm/main.h>
#include <iostream>
#include <filesystem>
#include "tactographview.h"
#include "gtk/pixbufops.h"
#include <math.h>
#include "generator.h"
#include "positiongen.h"
#include "visualizewidget.h"
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace latero::graphics {

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


class MaskView : public Gtk::AspectFrame
{
public:
	MaskView(const latero::Tactograph* dev, MaskPtr peer) :
 		Gtk::AspectFrame(0.5, 0.5, dev->GetSurfaceWidth()/dev->GetSurfaceHeight(), false),
		peer_(peer)
	{
    	drawingArea_.set_draw_func(sigc::mem_fun(*this, &MaskView::OnDraw));
		set_child(drawingArea_);
		drawingArea_.set_vexpand();

		drawingArea_.signal_resize().connect([this](int, int){ RefreshBackground(); });

		Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &MaskView::OnCheckPeer),
			(uint)333, Glib::PRIORITY_DEFAULT_IDLE);
	}

	virtual ~MaskView() {};

	void OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
	{
		if (width <= 0 || height <= 0)
			return;

    	if (bg_)
        	Gdk::Cairo::set_source_pixbuf(cr, bg_, 0, 0);
        else
			cr->set_source_rgb(1.0, 1.0, 1.0);
		cr->paint();
	}

protected:
	bool OnCheckPeer()
	{
		if (bgUpdateTime_.is_not_a_date_time() || (peer_->GetLastModified() > bgUpdateTime_))
			RefreshBackground();
		return true;
	}

	void RefreshBackground()
	{
		int w = drawingArea_.get_width();
		int h = drawingArea_.get_height();
		if (w <= 0 || h <= 0)
			return;
		bgUpdateTime_ = boost::posix_time::microsec_clock::universal_time();
		bg_ = peer_->GetIllustration(w, h);
		drawingArea_.queue_draw();
	}

private:
	MaskPtr peer_;
	boost::posix_time::ptime bgUpdateTime_;
	Glib::RefPtr<Gdk::Pixbuf> bg_;
	Gtk::DrawingArea drawingArea_;
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

		auto pOpenButton = Gtk::make_managed<Gtk::Button>();
		pOpenButton->set_icon_name("document-open");

		auto pReloadButton = Gtk::make_managed<Gtk::Button>();
		pReloadButton->set_icon_name("view-refresh");

		auto *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		box->append(fileEntry_);
		box->set_hexpand();
		fileEntry_.set_hexpand();
		box->append(*pOpenButton);
		box->append(*pReloadButton);
		box->append(*Gtk::make_managed<MaskSrcTypeCheck>(peer));
		set_child(*box);
 
		pOpenButton->signal_clicked().connect(sigc::mem_fun(*this, &MaskImageWidget::OnOpen));
		pReloadButton->signal_clicked().connect(sigc::mem_fun(*this, &MaskImageWidget::OnReload));
	}
	virtual ~MaskImageWidget() {};

protected:
	void OnOpen()
	{
		std::string dir;
		std::string currentFile = peer_->GetImgFile();
		if (currentFile.empty())
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
		for (auto& ext : {"png","jpg","jpeg","gif","bmp","tiff","tif","webp","svg"})
			filter->add_suffix(ext);
		auto filters = Gio::ListStore<Gtk::FileFilter>::create();
		filters->append(filter);

		auto dialog = Gtk::FileDialog::create();
		dialog->set_title("Please select an image file.");
		dialog->set_initial_folder(Gio::File::create_for_path(dir));
		dialog->set_filters(filters);
		dialog->set_default_filter(filter);

		auto* win = dynamic_cast<Gtk::Window*>(get_root());
		dialog->open(*win, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
			try {
				auto file = dialog->open_finish(result);
				std::string filename = file->get_path();
				fileEntry_.set_text(filename);
				peer_->SetImage(filename, false); // TODO
			} catch (const Gtk::DialogError&) {}
		});
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
		gtk::NumWidget *wWidget = Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, wRelAdj_, 0, units::percent,"width");
		wWidget->AddUnits(units::mm, wAbsAdj_, 0);
		wWidget->SelectUnits(peer->GetWidthUnits());
		append(*wWidget);
		wRelAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnRelWidthChanged));
		wAbsAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnAbsWidthChanged));
		wWidget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &MaskSizeCtrl::OnWidthUnitsChanged));

		gtk::NumWidget *hWidget = Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,hRelAdj_,0, units::percent,"height");
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
		
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		box->append(centeredRadio_);
		box->append(freeRadio_);
		box->append(posWidget_);
		posWidget_.set_hexpand();
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
		append(*Gtk::make_managed<Gtk::Label>("Default "));
		append(*Gtk::make_managed<Gtk::SpinButton>(adj_));
		append(*Gtk::make_managed<Gtk::Label>("%"));
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
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto checkbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);	
	auto maskImageWidget = Gtk::make_managed<MaskImageWidget>(peer);
	auto maskBlurCheck = Gtk::make_managed<MaskBlurCheck>(peer);
	auto maskLockAspectRatioCheck = Gtk::make_managed<MaskLockAspectRatioCheck>(peer);
	auto refMaxWidget = Gtk::make_managed<MaskRefMaximizedCheck>(peer);
	auto maskDefaultCtrl = Gtk::make_managed<MaskDefaultCtrl>(peer);
	auto maskView = Gtk::make_managed<MaskView>(dev,peer);

	MaskSizeCtrl *sizeWidget = Gtk::make_managed<MaskSizeCtrl>(peer);
	ctrls_.push_back(sizeWidget);

	MaskPositionCtrl *posWidget = Gtk::make_managed<MaskPositionCtrl>(peer);
	ctrls_.push_back(posWidget);

	maskView->set_vexpand();
	posWidget->set_vexpand();
	sizeWidget->set_vexpand();
	checkbox->set_vexpand();
	maskBlurCheck->set_hexpand();
	maskLockAspectRatioCheck->set_hexpand();
	refMaxWidget->set_hexpand();
	maskDefaultCtrl->set_hexpand();
	box->set_hexpand();

	box->append(*maskImageWidget);
	box->append(*posWidget);
	box->append(*sizeWidget);
	box->append(*checkbox);

	checkbox->append(*maskBlurCheck);
	checkbox->append(*maskLockAspectRatioCheck);
	checkbox->append(*refMaxWidget);
	checkbox->append(*maskDefaultCtrl);

	append(*box);
	append(*maskView);	

 	refMaxWidget->signal_toggled().connect(sigc::mem_fun(*this, &MaskWidget::SynchFromPeer));
	sizeWidget->SignalChanged().connect(sigc::mem_fun(*this, &MaskWidget::SynchFromPeer));
}

void MaskWidget::SynchFromPeer()
{
	for (unsigned int i=0; i<ctrls_.size(); ++i)
		ctrls_[i]->SynchFromPeer();
}

} // namespace
