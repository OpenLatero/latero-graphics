#include "imagedropdown.h"
#include <iostream>

namespace latero::graphics::gtk {

ImageDropDown::ImageDropDown() :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	list_(Gtk::StringList::create({})),
	dropDown_(list_)
{
	auto factory = Gtk::SignalListItemFactory::create();
	factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		item->set_child(*Gtk::make_managed<Gtk::Image>());
	});
	factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& item) {
		auto pos = item->get_position();
		if (pos < items_.size())
			if (auto image = dynamic_cast<Gtk::Image*>(item->get_child()))
				image->set(items_[pos].second);
	});
	dropDown_.set_factory(factory);
	dropDown_.set_list_factory(factory);
	append(dropDown_);
	dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &ImageDropDown::OnDropDownChanged));
}

ImageDropDown::~ImageDropDown()
{
}

void ImageDropDown::Append(int id, std::string imgfile)
{
	try
	{
		items_.push_back({id, Gdk::Pixbuf::create_from_file(imgfile)});
		list_->append("");
	}
	catch (Glib::Error &e)
	{
		std::cerr << "ImageDropDown::Append() " << e.what() << "\n";
	}
}

void ImageDropDown::SetActive(int id)
{
	for (guint i = 0; i < items_.size(); ++i)
		if (items_[i].first == id) { dropDown_.set_selected(i); return; }
}

void ImageDropDown::OnDropDownChanged()
{
	guint pos = dropDown_.get_selected();
	if (pos < items_.size())
		signalChanged_(items_[pos].first);
}

sigc::signal<void(int)> ImageDropDown::SignalChanged()
{
	return signalChanged_;
};

} // namespace
