#pragma once

#include <gdkmm/pixbuf.h>

// todo: turn this in a class?
// todo: are any of the ops implemented in GTKMM?

namespace latero::graphics::gtk {


class PixbufOps
{
public:
	/**
	 * Copies the alpha channel of src to dest. Buffers must have the same size.
	 * @warning Not well debugged. Could probably be optimed.
 	 */
	//static void CopyAlpha(Glib::RefPtr<Gdk::Pixbuf> &dest, const Glib::RefPtr<Gdk::Pixbuf> &src);

	static void CompositeMult(Glib::RefPtr<Gdk::Pixbuf> &dest, const Glib::RefPtr<Gdk::Pixbuf> &src, bool invertDest);
	static void Multiply(Glib::RefPtr<Gdk::Pixbuf> &dest, float r, float g, float b);
	static void Invert(Glib::RefPtr<Gdk::Pixbuf> &dest);
	static void Superpose(Glib::RefPtr<Gdk::Pixbuf> &dest, const Glib::RefPtr<Gdk::Pixbuf> &src);

	static void ShiftRange(Glib::RefPtr<Gdk::Pixbuf> &img, int min, int max);

	/** @warning: do not use repeatedly, not efficient */
	static void SetPixel(Glib::RefPtr<Gdk::Pixbuf> &dest, int x, int y, float r, float g, float b);

	/**
	 * Copies channel SRC to channel DEST. For example, CopyChannel(img,0,3) would copy the R value to the alpha channel.
	 */
	static void CopyChannel(Glib::RefPtr<Gdk::Pixbuf> &img, int src, int dest);
};

} // namespace

