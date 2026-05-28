#include "modulatordropdown.h"
#include "modulator.h"
#include "dirmodulator.h"
#include "velmodulator.h"

namespace latero::graphics {

void ModulatorDropDown::Append(ModulatorPtr mod)
{
	modList_.push_back(mod);
	Glib::RefPtr<Gdk::Pixbuf> img = mod->GetIllustration(50,50);
	if ((img->get_width() != 50) || (img->get_height() != 50))
		img = img->scale_simple(50,50,Gdk::InterpType::BILINEAR);
	imgList_.push_back(img);
	list_->append("");
}

void ModulatorDropDown::SetActive(ModulatorPtr mod)
{
	if (!mod) return;

	for (unsigned int i=0; i<modList_.size(); ++i)
	{
		if (modList_[i] == mod)
		{
			signalEnable_ = false;
			dropDown_.set_selected(i);
			signalEnable_ = true;
			return;
		}
	}

	Append(mod);
	SetActive(mod);
}

void ModulatorDropDown::OnDropDownChanged()
{
	guint i = dropDown_.get_selected();
	if (i < modList_.size() && signalEnable_)
		signalModulatorChanged_(modList_[i]);
}

ModulatorDropDown::ModulatorDropDown(ModulatorPtr mod) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	list_(Gtk::StringList::create({})),
	dropDown_(list_),
	dev_(mod->Dev()),
	signalEnable_(true)
{
	auto factory = Gtk::SignalListItemFactory::create();
	factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		item->set_child(*Gtk::make_managed<Gtk::Image>());
	});
	factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& item) {
		auto pos = item->get_position();
		if (pos < imgList_.size())
			if (auto image = dynamic_cast<Gtk::Image*>(item->get_child()))
				image->set(imgList_[pos]);
	});
	dropDown_.set_factory(factory);
	dropDown_.set_list_factory(factory);

	Append(DirModulator::Create(dev_));
	DirModulatorPtr symDirmod = DirModulator::Create(dev_);
	symDirmod->SetSymmetric(true);
	Append(symDirmod);
	Append(VelModulator::Create(dev_));

	SetActive(mod);

	dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &ModulatorDropDown::OnDropDownChanged));
	append(dropDown_);
}

} // namespace

