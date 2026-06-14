#include "tactiledisplayview.h"
#include "generator.h"
#include <iostream>

namespace latero::graphics {

TactileDisplayView::TactileDisplayView(const latero::TactileDisplay *dev, latero::graphics::GeneratorPtr gen) :
 	Gtk::AspectFrame(0.5, 0.5, dev->GetWidth()/dev->GetHeight(), false),
	peer_(gen),
	dev_(dev),
	tdState_(dev->GetFrameSizeX(), dev->GetFrameSizeY()),
	tdPainter_(dev_)
{
	set_child(drawingArea_);
	drawingArea_.set_expand();

	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &TactileDisplayView::RefreshCursor),
		(uint)33, // ms
		Glib::PRIORITY_DEFAULT_IDLE);

	drawingArea_.set_draw_func(sigc::mem_fun(*this, &TactileDisplayView::OnDraw));
}


TactileDisplayView::~TactileDisplayView()
{
}


void TactileDisplayView::OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
	if ((width<=0)||(height<=0))
		return;

	double mmTDWidth = dev_->GetWidth() * 1.5;
	double mmTDHeight = dev_->GetHeight() * 1.5;

	cr->save();
    cr->scale(width / mmTDWidth, height / mmTDHeight); // scale to mm
	cr->translate(mmTDWidth/2, mmTDHeight/2);
	tdPainter_.Paint(cr, tdState_);
	cr->restore();
}





bool TactileDisplayView::RefreshCursor()
{
	if (peer_)
	{
		tdState_ = peer_->GetLatestFrame();
    	drawingArea_.queue_draw();
	}
	return true;
}


void TactileDisplayView::SetGenerator(latero::graphics::GeneratorPtr gen)
{
	peer_ = gen;
}

} // namespace