#pragma once

#include <gtkmm.h>
#include <latero/tactograph.h>
#include "gtk/animation.h"
#include "generatorfwd.h"
#include "tactiledisplayview.h"

namespace latero::graphics {

class CursorLayer
{
public:
	CursorLayer(const latero::Tactograph *dev);
	~CursorLayer() {};
	void Draw(const Cairo::RefPtr<Cairo::Context> &mmContext, double dpmm_x);

	void SetAnimate(bool v=true) { animate_ = v; }
	void SetEnable(bool v=true) { enable_ = v; }

	void SetDisplayState(const Point &position, double angle, const latero::BiasedImg &frame);

protected:
	Cairo::RefPtr<Cairo::Pattern> GetCursorDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext, double dpmm_x);
	Cairo::RefPtr<Cairo::Pattern> GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext);

	Point tdPos_;
	double tdAngle_;
	latero::BiasedImg tdState_;

	bool enable_;
	bool animate_;
	const latero::Tactograph *dev_;
	TactileDisplayPainter tdPainter_;
};

class TactographView : public Gtk::AspectFrame
{
public:
	/**
	 * Constructor.
	 * @param dev A const pointer to the Tactograph device used by the widget for dimensions and other specifications.
	 * @param gen An optional pointer to the currently active Generator.
	 * @param refreshBackground If true, TactographView will periodically check if the generator
	 * has been modified and will update the background if necessary.
	 */
	TactographView(const latero::Tactograph *dev, GeneratorPtr gen = GeneratorPtr(), bool refreshBackground=false);

	/**
	 * Destructor.
	 */
	virtual ~TactographView();

	/**
	 * Update the current generator. Triggers an update of the background.
	 * @param gen The new generator to use.
	 */
	void SetGenerator(GeneratorPtr gen);

	void ShowCursor(bool v = true);
	void AnimateCursor(bool v = true);

	void ClearBackground(guint32 pixel);
	void SetBackground(latero::graphics::gtk::Animation &anim);
	void SetBackground(Glib::RefPtr<Gdk::Pixbuf> buf);

protected:
	void RefreshBackground();
	bool RefreshCursor();
    Point GetClickPos(double x, double y);
    void OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

	void OnEdit();
	void OnSave();
	void OnSaveCanvas();
	void OnSaveCanvasAs();
	void OnVisualize();
	void OnClick(int n_press, double x, double y);
	void CreatePopupMenu();

	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	bool OnCheckPeer();

	latero::graphics::gtk::Animation anim_;
	const latero::Tactograph *dev_;
	CursorLayer cursorLayer_;
	Gtk::DrawingArea drawingArea_;

private:
	GeneratorPtr peer_;
	boost::posix_time::ptime bgUpdateTime_;


};

} // namespace

