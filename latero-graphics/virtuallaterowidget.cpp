#include "virtuallaterowidget.h"
#include <laterographics/generator.h>
#include <iostream>

namespace latero::graphics {

VirtualLateroWidget::VirtualLateroWidget(const latero::TactileDisplay *dev, latero::graphics::GeneratorPtr gen) :
 	Gtk::AspectFrame(0.5, 0.5, dev->GetWidth()/dev->GetHeight(), false),
	peer_(gen),
	dev_(dev),
	tdState_(dev->GetFrameSizeX(), dev->GetFrameSizeY())
{
	set_child(drawingArea_);
	drawingArea_.set_expand();

	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &VirtualLateroWidget::RefreshCursor),
		(uint)33, // ms
		Glib::PRIORITY_DEFAULT_IDLE);

	drawingArea_.set_draw_func(sigc::mem_fun(*this, &VirtualLateroWidget::OnDraw));
}


VirtualLateroWidget::~VirtualLateroWidget()
{
}


void VirtualLateroWidget::OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
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


Cairo::RefPtr<Cairo::Pattern> VirtualLateroWidget::GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext)
{
	mmContext->push_group();

	double tdw = dev_->GetWidth()*1.4;
	double tdh = dev_->GetHeight()*1.2;
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

	float motionRange = 0.7 * dev_->GetPitchX();
	int hPiezo = dev_->GetContactorSizeY();

	mmContext->set_source_rgb(1.0, 0.0, 0.0);
	for (uint j=0; j< dev_->GetFrameSizeY(); ++j)
	{
		for (uint i=0; i< dev_->GetFrameSizeX(); ++i)
		{
			latero::graphics::Point p = dev_->GetActuatorOffset(i,j);
			float x = p.x + (0.5-tdState_.Get(i,j))*motionRange;
			mmContext->move_to(x, p.y - 0.5*hPiezo);
	        mmContext->line_to(x, p.y + 0.5*hPiezo);
			mmContext->set_source_rgb(1.0, 0.0, 0.0);
			mmContext->set_line_width(0.3*dev_->GetPitchX());
			mmContext->stroke();
		}
	}
	return mmContext->pop_group();
}


bool VirtualLateroWidget::RefreshCursor()
{
	if (peer_)
	{
		tdState_ = peer_->GetLatestFrame();
    	drawingArea_.queue_draw();
	}
	return true;
}


void VirtualLateroWidget::SetGenerator(latero::graphics::GeneratorPtr gen)
{
	peer_ = gen;
}

} // namespace