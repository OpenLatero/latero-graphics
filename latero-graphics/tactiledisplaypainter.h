#pragma once

#include <gtkmm.h>
#include <latero/tactileimg.h>
#include <latero/tactiledisplay.h>

namespace latero::graphics {

class TactileDisplayPainter
{
public:
	TactileDisplayPainter(const latero::TactileDisplay *dev);
	void Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, bool drawOutline=true);
protected:
	const latero::TactileDisplay *dev_;
};

} // namespace
