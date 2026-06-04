#include "patterntexturewidget.h"
#include "patterntexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../../graphics/patternpreview.h"
#include "../../graphics/patterncreatordialog.h"


namespace latero::graphics {

class PatternTextureGridWidthCtrl : public Gtk::Box
{
public:
	PatternTextureGridWidthCtrl(PatternTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetGridWidth(),0.1,500)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 3, units::mm, "grid width"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PatternTextureGridWidthCtrl::OnChanged));
	}
	virtual ~PatternTextureGridWidthCtrl() {};
protected:
	void OnChanged()  { peer_->SetGridWidth(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	PatternTexturePtr peer_;
};

class PatternTextureGridHeightCtrl : public Gtk::Box
{
public:
	PatternTextureGridHeightCtrl(PatternTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetGridHeight(),0.1,500)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 3, units::mm, "grid height"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PatternTextureGridHeightCtrl::OnChanged));
	}
	virtual ~PatternTextureGridHeightCtrl() {};
protected:
	void OnChanged()  { peer_->SetGridHeight(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	PatternTexturePtr peer_;
};


PatternTextureWidget::PatternTextureWidget(PatternTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	PatternPtr pattern = peer->GetPattern();
	auto loadButton = Gtk::make_managed<Gtk::Button>();
	loadButton->set_icon_name("document-open");

	auto patternTextureGridWidthCtrl = Gtk::make_managed<PatternTextureGridWidthCtrl>(peer);
	auto patternTextureGridHeightCtrl = Gtk::make_managed<PatternTextureGridHeightCtrl>(peer);
	auto textureAdvancedButton = Gtk::make_managed<TextureAdvancedButton>(peer);
	auto patternWidget = pattern->CreateWidget(pattern);
	auto textureInvertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer);
	auto textureAmplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);
	auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

	patternTextureGridWidthCtrl->set_hexpand();
	patternTextureGridHeightCtrl->set_hexpand();
	patternHolder_.set_vexpand();
	textureAmplitudeCtrl->set_vexpand();

	Gtk::Box *topbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	topbox->append(*patternTextureGridWidthCtrl);
	topbox->append(*patternTextureGridHeightCtrl);
	topbox->append(*textureAdvancedButton);
	topbox->append(*loadButton);

	auto mainbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	mainbox->append(*topbox);
	mainbox->append(patternHolder_);
	patternHolder_.set_child(*patternWidget);
	
	auto sidebox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	sidebox->append(*textureInvertCtrl);
	sidebox->append(*textureAmplitudeCtrl);
	append(*sidebox);
	append(*mainbox);
	append(*patternPreview);

	loadButton->signal_clicked().connect(sigc::mem_fun(*this, &PatternTextureWidget::OnLoad));
}

void PatternTextureWidget::OnLoad()
{
	auto dlg = new PatternCreatorDialog(peer_->Dev());
	dlg->signal_response().connect([this, dlg](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
		{
			PatternPtr newPattern = dlg->CreatePattern();
			if (newPattern)
			{
				Gtk::Widget *wp = patternHolder_.get_child();
				patternHolder_.unset_child();
				delete wp;
				peer_->SetPattern(newPattern);
				patternHolder_.set_child(*newPattern->CreateWidget(newPattern));
			}
		}
		delete dlg;
	});
	dlg->present();
}







PatternTextureAdvancedWidget::PatternTextureAdvancedWidget(PatternTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	PatternPtr pattern = peer->GetPattern();

	auto gridWidthCtrl = Gtk::make_managed<PatternTextureGridWidthCtrl>(peer);
	auto gridHeightCtrl = Gtk::make_managed<PatternTextureGridHeightCtrl>(peer);
	auto loadButton = Gtk::make_managed<Gtk::Button>();
	auto motionCtrl = Gtk::make_managed<TextureMotionCtrl>(peer);
	auto invertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer);
	auto amplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);

	loadButton->set_icon_name("document-open");
	gridWidthCtrl->set_hexpand();
	gridHeightCtrl->set_hexpand();
	motionCtrl->set_vexpand();
	patternHolder_.set_vexpand();
	amplitudeCtrl->set_vexpand();

	auto topbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	topbox->append(*gridWidthCtrl);
	topbox->append(*gridHeightCtrl);
	topbox->append(*loadButton);

	auto mainbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	mainbox->append(*topbox);
	mainbox->append(*motionCtrl);
	mainbox->append(patternHolder_);
	patternHolder_.set_child(*pattern->CreateWidget(pattern));
	
	auto sidebox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	sidebox->append(*invertCtrl);
	sidebox->append(*amplitudeCtrl);

	append(*sidebox);
	append(*mainbox);
	append(*Gtk::make_managed<PatternPreview>(peer));

	loadButton->signal_clicked().connect(sigc::mem_fun(*this, &PatternTextureAdvancedWidget::OnLoad));
}

void PatternTextureAdvancedWidget::OnLoad()
{
	auto dlg = new PatternCreatorDialog(peer_->Dev());
	dlg->signal_response().connect([this, dlg](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
		{
			PatternPtr newPattern = dlg->CreatePattern();
			if (newPattern)
			{
				Gtk::Widget *wp = patternHolder_.get_child();
				patternHolder_.unset_child();
				delete wp;
				peer_->SetPattern(newPattern);
				patternHolder_.set_child(*newPattern->CreateWidget(newPattern));
			}
		}
		delete dlg;
	});
	dlg->present();
}

} // namespace
