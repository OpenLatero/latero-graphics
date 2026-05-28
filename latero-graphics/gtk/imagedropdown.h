#pragma once

#include <gtkmm.h>

namespace latero::graphics::gtk {

/**
 * This widget implements a DropDown that displays a list of images. A numerical ID is associated with
 * each image.
 */
class ImageDropDown : public Gtk::Box
{
public:
	/** ctor */
	ImageDropDown();

	/** dtor */
	virtual ~ImageDropDown();

	/** Append an image with its associated ID. */
	void Append(int id, std::string imgfile);

	/** Set activce the image with a given ID. */
	void SetActive(int id);

	/** Callback for changes to selected image. Image ID is passed as an argument. */
	sigc::signal<void(int)> SignalChanged();

protected:

	void OnDropDownChanged();

	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	std::vector<std::pair<int, Glib::RefPtr<Gdk::Pixbuf>>> items_;
	sigc::signal<void(int)> signalChanged_;
};

} // namespace

