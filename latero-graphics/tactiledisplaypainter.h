#pragma once

#include <gtkmm.h>
#include <latero/tactileimg.h>
#include <latero/tactiledisplay.h>

namespace latero::graphics {

/** @todo should there be a TactographPainter as well? should the class know the tactile display state? */
class TactileDisplayPainter
{
public:
	/**
	 * Constructor.
	 * @param dev Tactile display handle
	 */
	TactileDisplayPainter(const latero::TactileDisplay *dev);
	
	/**
	 * Draw the tactile display in a mm-scaled drawing context. The tactile display rendering automatically changes
	 * from a detailed animation to a simple rectangle depending on its size in pixels. If specified, the tactile display
	 * is centered on position (\p x, \p y) and drawn at \p angle. If not, the tactile is centered on the current position (0,0).
	 * 
	 * @param mmContext Drawing context scaled to mm.
	 * @param tdState Latest state of the tactile display.
	 * @param x (optional) Position (x) at which to draw the center of the tactile display (in mm).
	 * @param y (optional) Position (y) at which to draw the center of the tactile display (in mm).
	 * @param angle (optional) Angle at which to draw the tactile display.
	 */
	void Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, double x=0, double y=0, double angle=0);
	
	/**
	 * Enable or disable drawing when \ref Paint is called.
	 * @param v Enable if true.
	 */
	void Enable(bool v) { drawEnable_=v; };

	/**
	 * If enabled, the tactile display will be drawn with an outline and an opaque background when \ref Paint is called.
	 * @param v Enable if true.
	 */
	void EnableOutline(bool v) { drawOutline_=v; };

	/**
	 * Set whether the tactile display should always be drawn as a simple rectangle no matter its size in pixels.
	 * @param v Always drawn as a simple rectangle if true.
	 */
	void ForceSimple(bool v) { forceSimple_=v; };

protected:
	/**
	 * Paint a detailed view of the tactile display centered at current location.
	 * @param mmContext Drawing context scaled to mm.
	 * @param tdState Latest state of the tactile display.
	 */
	void PaintDetailed(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState);

	/**
	 * Paint a simplified view of the tactile display centered at current location.
	 * @param mmContext Drawing context scaled to mm.
	 */
	void PaintSimple(const Cairo::RefPtr<Cairo::Context> &mmContext);

	bool drawEnable_;  ///< If false, nothing happens when \ref Paint is called.
	bool drawOutline_; ///< If true, the tactile display is rendered with an outline and an opaque background
	bool forceSimple_; ///< If true, always render the simplified view of the tactile display.

	const latero::TactileDisplay *dev_; ///< Tactile display handle
};

} // namespace
