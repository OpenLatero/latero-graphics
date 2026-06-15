#pragma once

#include <gtkmm.h>
#include <latero/tactileimg.h>
#include <latero/tactiledisplay.h>

namespace latero::graphics {

class TactileDisplayPainter
{
public:
	TactileDisplayPainter(const latero::TactileDisplay *dev);
	void Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState);
	void Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, double x, double y, double angle);
	void EnableOutline(bool v) { drawOutline_=v; };
protected:
	bool drawOutline_;
	const latero::TactileDisplay *dev_;
};

} // namespace
