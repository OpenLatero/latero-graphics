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

#ifndef LATERO_GRAPHICS_VIRTUAL_SURFACE_WIDGET_H
#define LATERO_GRAPHICS_VIRTUAL_SURFACE_WIDGET_H

#include <gtkmm/uimanager.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/alignment.h>
#include <gtkmm/aspectframe.h>
#include <gtkmm/eventbox.h>
#include <latero/tactileimg.h>
#include <latero/tactograph.h>
#include "gtk/animation.h"
#include "point.h"
#include "generatorfwd.h"

namespace latero {
namespace graphics {

/** Use VirtualSurfaceWidget instead! */
class VirtualSurfaceArea : public Gtk::DrawingArea
{
public:
	VirtualSurfaceArea(const latero::Tactograph *dev);
	virtual ~VirtualSurfaceArea();
	
	void Clear(guint32 pixel);
	void Set(latero::graphics::gtk::Animation &anim);
	void Set(Glib::RefPtr<Gdk::Pixbuf> buf);
	latero::graphics::gtk::Animation GetIllustration();
	void SetRounded(bool v=true);

	void SetDisplayState(const Point &position, double angle, const latero::BiasedImg &frame);

	void ShowBorder(bool v = true);
	void ShowCursor(bool v = true);
	void AnimateCursor(bool v = true);

	inline uint GetWidth() { return get_allocation().get_width(); };
	inline uint GetHeight() { return get_allocation().get_height(); };

	void DisablePopup() { disablePopup_=true; }
	void EnablePopup() { disablePopup_=false; }

	Cairo::RefPtr<Cairo::Pattern> GetDisplayDrawing(Cairo::RefPtr<Cairo::Context> &mmContext);

	// tmp
	inline Point GetDisplayPos() const { return tdPos_; }

protected:
	void CreatePopupMenu();
	bool OnClick(GdkEventButton* event);
	void OnSave();
    bool OnButtonPress(GdkEventButton* event);
    bool OnMotionNotify(GdkEventMotion *event);
    bool OnButtonRelease(GdkEventButton* event);
    Point GetClickPos(double x, double y);

    
	/** @returns transparency mask for the widget's shape */
	Glib::RefPtr<Gdk::Bitmap> GetShapeCombineMask();

	// invalidate the entire window
	void Invalidate();

	virtual void on_size_allocate(Gtk::Allocation& allocation);

	/** dots per mm in x */
	inline double dpmm_x() { return GetWidth() / dev_->GetSurfaceWidth(); }

	/** dots per mm in y */
	inline double dpmm_y() { return GetHeight() / dev_->GetSurfaceHeight(); }

	Gdk::Rectangle GetDisplayFootprint(uint border);
	void DrawBorder(Cairo::RefPtr<Cairo::Context> &cr);
	void DrawBorderPath(Cairo::RefPtr<Cairo::Context> &cr);

	Cairo::RefPtr<Cairo::Pattern> GetCursorDrawing(Cairo::RefPtr<Cairo::Context> &cr);

	void DrawCursor(Cairo::RefPtr<Cairo::Context> &cr);

	virtual bool on_expose_event(GdkEventExpose* event);

protected:
    
	bool showCursor_, showBorder_, animateCursor_;

	latero::graphics::gtk::Animation anim_;

	Glib::RefPtr<Gtk::UIManager> uiManager_;
	const latero::Tactograph *dev_;

	Point tdPos_;
	double tdAngle_;
	latero::BiasedImg tdState_;
	bool rounded_;
	bool disablePopup_;
};

/**
 * This widget represents the virtual surface explored by a tactile display. It is implement as an AspectFrame enclosing a 
 * DrawingArea so that the aspect ratio can be maintained.
 */
class BaseVirtualSurfaceWidget : public Gtk::EventBox
{
public:
	BaseVirtualSurfaceWidget(const latero::Tactograph *dev) :
 		frame_("", 0.5, 0.5, dev->GetSurfaceWidth()/dev->GetSurfaceHeight(), false),
		surface_(dev)
	{
		add(frame_);
		frame_.unset_label(); // this is necessary to remove blank above surface
		frame_.set_shadow_type(Gtk::SHADOW_NONE); // this removes the border
		frame_.add(surface_);
	}

	virtual ~BaseVirtualSurfaceWidget()
	{
	}
	
	inline void Clear(guint32 pixel) {
		surface_.Clear(pixel);
	}

	inline void Set(latero::graphics::gtk::Animation &anim) {
		surface_.Set(anim);
	}

	inline void Set(Glib::RefPtr<Gdk::Pixbuf> buf) {
		surface_.Set(buf);
	}

	inline void SetRounded(bool v=true) {
		surface_.SetRounded(v);
	}

	inline latero::graphics::gtk::Animation GetIllustration() {
		return surface_.GetIllustration();
	}

	inline void SetDisplayState(const Point &position, double angle, const latero::BiasedImg &frame) {
		surface_.SetDisplayState(position, angle, frame);
	}

	inline void ShowBorder(bool v = true) {
		surface_.ShowBorder(v);
	}

	inline void ShowCursor(bool v = true) {
		surface_.ShowCursor(v);
	}

	inline uint GetWidth() {
		return surface_.GetWidth();
	}

	inline uint GetHeight() {
		return surface_.GetHeight();
	}

	inline void AnimateCursor(bool v = true) {
		surface_.AnimateCursor(v);
	}

	inline Cairo::RefPtr<Cairo::Pattern> GetDisplayDrawing(Cairo::RefPtr<Cairo::Context> &mmContext) {
		return surface_.GetDisplayDrawing(mmContext);
	}

protected:
	Gtk::AspectFrame frame_;
	VirtualSurfaceArea surface_;
};

class VirtualSurfaceWidget : public BaseVirtualSurfaceWidget
{
public:
	VirtualSurfaceWidget(const latero::Tactograph *dev, GeneratorPtr gen = GeneratorPtr(), bool refreshBackground=false);
	virtual ~VirtualSurfaceWidget();
	void SetGenerator(GeneratorPtr gen);
	void RefreshBackground();
	bool RefreshCursor();

	// tmp
	Point GetDisplayPos();
protected:
	void OnEdit();
	void OnSave();
	void OnSaveCanvas();
	void OnSaveCanvasAs();
	void OnVisualize();
	bool OnClick(GdkEventButton* event);
	void CreatePopupMenu();

	Glib::RefPtr<Gtk::UIManager> uiManager_;
	bool OnCheckPeer();
	virtual void on_size_allocate(Gtk::Allocation& allocation);

private:
	GeneratorPtr peer_;
	boost::posix_time::ptime bgUpdateTime_;
};

} // namespace graphics
} // namespace latero

#endif
