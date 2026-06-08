#pragma once

#include <gtkmm.h>
#include <latero/tactograph.h>
#include "gtk/animation.h"
#include "generatorfwd.h"

namespace latero::graphics {

/** Use VirtualSurfaceWidget instead! */
class VirtualSurfaceArea : public Gtk::AspectFrame
{
public:
	VirtualSurfaceArea(const latero::Tactograph *dev);
	virtual ~VirtualSurfaceArea();

	void ShowCursor(bool v = true);
	void AnimateCursor(bool v = true);


	void Clear(guint32 pixel);
	void Set(latero::graphics::gtk::Animation &anim);
	void Set(Glib::RefPtr<Gdk::Pixbuf> buf);



protected:
	void SetDisplayState(const Point &position, double angle, const latero::BiasedImg &frame);

	Cairo::RefPtr<Cairo::Pattern> GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext);

	inline uint GetWidth() { return drawingArea_.get_width(); };
	inline uint GetHeight() { return drawingArea_.get_height(); };

    Point GetClickPos(double x, double y);

	// invalidate the entire window
	void Invalidate();

	/** dots per mm in x */
	inline double dpmm_x() { return GetWidth() / dev_->GetSurfaceWidth(); }

	/** dots per mm in y */
	inline double dpmm_y() { return GetHeight() / dev_->GetSurfaceHeight(); }

	Gdk::Rectangle GetDisplayFootprint(uint border);

	Cairo::RefPtr<Cairo::Pattern> GetCursorDrawing(const Cairo::RefPtr<Cairo::Context> &cr);

	void DrawCursor(const Cairo::RefPtr<Cairo::Context> &cr);

    void OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

protected:
    
	bool showCursor_, animateCursor_;

	latero::graphics::gtk::Animation anim_;

	const latero::Tactograph *dev_;

	Point tdPos_;
	double tdAngle_;
	latero::BiasedImg tdState_;

	Gtk::DrawingArea drawingArea_;
};

class VirtualSurfaceWidget : public VirtualSurfaceArea
{
public:
	VirtualSurfaceWidget(const latero::Tactograph *dev, GeneratorPtr gen = GeneratorPtr(), bool refreshBackground=false);
	virtual ~VirtualSurfaceWidget();
	void SetGenerator(GeneratorPtr gen);

	void RefreshBackground();
	bool RefreshCursor();

protected:
	


	void OnEdit();
	void OnSave();
	void OnSaveCanvas();
	void OnSaveCanvasAs();
	void OnVisualize();
	void OnClick(int n_press, double x, double y);
	void CreatePopupMenu();

	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	bool OnCheckPeer();

private:
	GeneratorPtr peer_;
	boost::posix_time::ptime bgUpdateTime_;
};

} // namespace

