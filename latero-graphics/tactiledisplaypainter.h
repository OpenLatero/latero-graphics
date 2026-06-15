#pragma once

#include <gtkmm.h>
#include <latero/tactileimg.h>
#include <latero/tactiledisplay.h>

namespace latero::graphics {

/** @todo should there be a TactographPainter as well? should the class know the tactile display state? */
class TactileDisplayPainter
{
public:
	TactileDisplayPainter(const latero::TactileDisplay *dev);
	
	void Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState);
	void Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, double x, double y, double angle);

	void PaintDetailed(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState);
	void PaintDetailed(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, double x, double y, double angle);
	
	void PaintSimple(const Cairo::RefPtr<Cairo::Context> &mmContext);
	void PaintSimple(const Cairo::RefPtr<Cairo::Context> &mmContext, double x, double y, double angle);
	
	void Enable(bool v) { drawEnable_=v; };
	void EnableOutline(bool v) { drawOutline_=v; };
	void EnableAnimate(bool v) { drawAnimate_=v; };
protected:
	bool drawEnable_;
	bool drawOutline_;
	bool drawAnimate_;
	const latero::TactileDisplay *dev_;
};

} // namespace
