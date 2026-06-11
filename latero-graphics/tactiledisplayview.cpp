#include "tactiledisplayview.h"
#include "generator.h"
#include <iostream>

namespace latero::graphics {

TactileDisplayView::TactileDisplayView(const latero::TactileDisplay *dev, latero::graphics::GeneratorPtr gen) :
 	Gtk::AspectFrame(0.5, 0.5, dev->GetWidth()/dev->GetHeight(), false),
	peer_(gen),
	dev_(dev),
	tdState_(dev->GetFrameSizeX(), dev->GetFrameSizeY())
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
	cr->set_source(GetDisplayDrawing(cr));
	cr->paint();
	cr->restore();
}

Cairo::RefPtr<Cairo::Pattern> TactileDisplayView::GetTactileDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::TactileDisplay *dev, const latero::BiasedImg &tdState)
{
	mmContext->push_group();	
	double tdw = dev->GetWidth()*1.4;
	double tdh = dev->GetHeight()*1.2;

	mmContext->rectangle(-tdw/2, -tdh/2, tdw, tdh);
	mmContext->set_source_rgb(1.0, 1.0, 1.0);
	mmContext->set_line_width(1.5);
	mmContext->fill_preserve();
	mmContext->set_line_width(1.5);
	mmContext->set_source_rgb(1.0, 1.0, 1.0);
	mmContext->stroke_preserve();
	mmContext->set_line_width(0.5);
	mmContext->set_source_rgb(1.0, 0.0, 0.0);
	mmContext->stroke();

	float motionRange = 0.7 * dev->GetPitchX();
	int hPiezo = dev->GetContactorSizeY();

	mmContext->set_source_rgb(1.0, 0.0, 0.0);
	for (uint j=0; j< dev->GetFrameSizeY(); ++j)
	{
		for (uint i=0; i< dev->GetFrameSizeX(); ++i)
		{
			latero::graphics::Point p = dev->GetActuatorOffset(i,j);
			float x = p.x + (0.5-tdState.Get(i,j))*motionRange;
			mmContext->move_to(x, p.y - 0.5*hPiezo);
	        mmContext->line_to(x, p.y + 0.5*hPiezo);
			mmContext->set_source_rgb(1.0, 0.0, 0.0);
			mmContext->set_line_width(0.3*dev->GetPitchX());
			mmContext->stroke();
		}
	}	
	return mmContext->pop_group();	
}

Cairo::RefPtr<Cairo::Pattern> TactileDisplayView::GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext)
{
	return GetTactileDisplayDrawing(mmContext, dev_, tdState_);
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