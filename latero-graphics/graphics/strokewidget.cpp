#include "strokewidget.h"
#include "stroke.h"
#include "../gtk/numwidget.h"
#include "../gratingwidget.h"
#include "../ridgewidget.h"
#include "texture/vibrotexturewidget.h"
#include "../oscillatorwidget.h"
#include "../grating.h"
#include "dotpatternwidget.h"

namespace latero::graphics {

class StrokeIntensityCtrl : public Gtk::Box
{
public:
	StrokeIntensityCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetIntensity()*100,0,100)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL,"stroke intensity", adj_, 1, "%"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeIntensityCtrl::OnChanged));
	}
	virtual ~StrokeIntensityCtrl() {};
protected:
	void OnChanged() { peer_->SetIntensity(adj_->get_value()/100); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	StrokePtr peer_;
};


class StrokeWidthCtrl : public Gtk::Box
{
public:
	StrokeWidthCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetWidth(),0,50.0)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL,"stroke width", adj_, 1, "mm"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeWidthCtrl::OnChanged));
	}
	virtual ~StrokeWidthCtrl() {};
protected:
	void OnChanged() { peer_->SetWidth(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;	
	StrokePtr peer_;
};



class StrokeGutterCheck : public Gtk::CheckButton
{
public:
	StrokeGutterCheck(StrokePtr peer) :
		Gtk::CheckButton("gutter"), peer_(peer)
	{
		set_active(peer_->GetGutterEnable());
		signal_toggled().connect(sigc::mem_fun(*this, &StrokeGutterCheck::OnChanged));
	}
	virtual ~StrokeGutterCheck() {}
protected:
	void OnChanged() { peer_->SetGutterEnable(get_active()); }
	StrokePtr peer_;
};



class StrokeTextureWidget : public gtk::CheckFrame
{
public:
	StrokeTextureWidget(StrokePtr peer) : gtk::CheckFrame(peer->GetTexture()->GetEnable(), "grating"), peer_(peer)
	{
		auto miscBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		auto amplitudeWidget = Gtk::make_managed<GratingAmplitudeWidget>(peer->GetTexture());
		auto invertCtrl = Gtk::make_managed<GratingInvertCtrl>(peer->GetTexture());
		auto pitchWidget = Gtk::make_managed<GratingPitchWidget>(peer->GetTexture());

		amplitudeWidget->set_hexpand();
		pitchWidget->set_hexpand();
		miscBox->set_hexpand();

		miscBox->append(*amplitudeWidget);
		miscBox->append(*invertCtrl);
		GetBox().append(*box);
		box->append(*pitchWidget);
		box->append(*miscBox);
	
		GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &StrokeTextureWidget::OnEnable));
	};
	virtual ~StrokeTextureWidget() {};
protected:
	void OnEnable() { peer_->GetTexture()->SetEnable(GetCheck().get_active()); };
	StrokePtr peer_;
};

class StrokeVibrationCtrl : public Gtk::Box
{
public:
	StrokeVibrationCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL)
	{
		gtk::CheckFrame *vib = Gtk::make_managed<OscillatorWidget>(peer->GetOscillator(), true);
		append(*vib);
	}
	virtual ~StrokeVibrationCtrl() {}
};




StrokeProfileWidget::StrokeProfileWidget(StrokePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto strokeWidthCtrl = Gtk::make_managed<StrokeWidthCtrl>(peer);
	auto strokeIntensityCtrl = Gtk::make_managed<StrokeIntensityCtrl>(peer);
	auto ridgeEdgeWidthScale = Gtk::make_managed<RidgeEdgeWidthScale>(peer->GetProfile());
	auto ridgeTextureCtrl = Gtk::make_managed<RidgeTextureCtrl>(peer->GetProfile());
	auto ridgeGraph = Gtk::make_managed<RidgeGraph>(peer->GetProfile(),300);

	strokeWidthCtrl->set_vexpand();
	strokeIntensityCtrl->set_vexpand();
	ridgeEdgeWidthScale->set_vexpand();
	ridgeTextureCtrl->set_vexpand();
	strokeWidthCtrl->set_hexpand();

	box->append(*strokeWidthCtrl);
	box->append(*strokeIntensityCtrl);
	box->append(*ridgeEdgeWidthScale);
	box->append(*ridgeTextureCtrl);

	append(*box);
	append(*ridgeGraph);
};


StrokeFillWidget::StrokeFillWidget(StrokePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL)
{
	Gtk::Frame *frame = Gtk::make_managed<Gtk::Frame>("general");
	frame->set_child(*Gtk::make_managed<StrokeGutterCheck>(peer));

	auto vibrationCtrl = Gtk::make_managed<StrokeVibrationCtrl>(peer);
	auto textureWidget = Gtk::make_managed<StrokeTextureWidget>(peer);

	frame->set_vexpand();
	vibrationCtrl->set_vexpand();
	textureWidget->set_vexpand();

	append(*frame);
	append(*vibrationCtrl);
	append(*textureWidget);

	frame->set_vexpand();
	vibrationCtrl->set_vexpand();
	textureWidget->set_vexpand();
};





class StrokeDotEnableCheck : public Gtk::CheckButton
{
public:
	StrokeDotEnableCheck(StrokePtr peer) :
		Gtk::CheckButton("enable"), peer_(peer)
	{
		set_active(peer_->GetDotEnable());
		signal_toggled().connect(sigc::mem_fun(*this, &StrokeDotEnableCheck::OnChanged));
	}
	virtual ~StrokeDotEnableCheck() {}
protected:
	void OnChanged() { peer_->SetDotEnable(get_active()); }
	StrokePtr peer_;
};


StrokeDottedWidget::StrokeDottedWidget(StrokePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL)
{
	append(*Gtk::make_managed<StrokeDotEnableCheck>(peer));
	DotPatternPtr p = peer->GetDots();
	if (p) append(*Gtk::make_managed<DotPatternMinSpacingCtrl>(p));
};

} // namespace

