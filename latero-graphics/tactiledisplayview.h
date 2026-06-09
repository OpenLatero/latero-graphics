#pragma once

#include <gtkmm.h>
#include <latero/tactileimg.h>
#include <latero/tactiledisplay.h>
#include "generatorfwd.h"

namespace latero::graphics {

class TactileDisplayView : public Gtk::AspectFrame
{
public:
	TactileDisplayView(const latero::TactileDisplay *dev, latero::graphics::GeneratorPtr gen = latero::graphics::GeneratorPtr());
	virtual ~TactileDisplayView();
	void SetGenerator(latero::graphics::GeneratorPtr gen);
	
protected:
	bool RefreshCursor();
	Cairo::RefPtr<Cairo::Pattern> GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext);
	void OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

	latero::graphics::GeneratorPtr peer_;
	boost::posix_time::ptime bgUpdateTime_;
    const latero::TactileDisplay *dev_;
	latero::BiasedImg tdState_;
	Gtk::DrawingArea drawingArea_;
};

} // namespace