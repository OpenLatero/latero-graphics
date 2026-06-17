#include "tactiledisplaypainter.h"
#include "point.h"

namespace latero::graphics {

TactileDisplayPainter::TactileDisplayPainter(const latero::TactileDisplay *dev) : 
	dev_(dev),
	drawOutline_(true),
	forceSimple_(false),
	drawEnable_(true)
{
}


void TactileDisplayPainter::Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, double x, double y, double angle)
{
	if (!drawEnable_)
		return;

	mmContext->save();
	if ((x!=0)||(y!=0))
		mmContext->translate(x, y);
	if (angle!=0)
		mmContext->rotate(angle);

	double tdWidthPix = dev_->GetWidth();
	double tdHeightPix = dev_->GetWidth(); 
	mmContext->user_to_device_distance(tdWidthPix, tdHeightPix);
	if ((tdWidthPix < 15) || forceSimple_)
		PaintSimple(mmContext);
	else
		PaintDetailed(mmContext, tdState);


	mmContext->restore();
}


void TactileDisplayPainter::PaintDetailed(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState)
{
	mmContext->push_group();
	double tdw = dev_->GetWidth()*1.4;
	double tdh = dev_->GetHeight()*1.2;

	if (drawOutline_)
	{
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
	}

	float motionRange = 0.7 * dev_->GetPitchX();
	int hPiezo = dev_->GetContactorSizeY();

	mmContext->set_source_rgb(1.0, 0.0, 0.0);
	for (uint j=0; j< dev_->GetFrameSizeY(); ++j)
	{
		for (uint i=0; i< dev_->GetFrameSizeX(); ++i)
		{
			Point p = dev_->GetActuatorOffset(i,j);
			float x = p.x + (0.5-tdState.Get(i,j))*motionRange;
			mmContext->move_to(x, p.y - 0.5*hPiezo);
			mmContext->line_to(x, p.y + 0.5*hPiezo);
			mmContext->set_source_rgb(1.0, 0.0, 0.0);
			mmContext->set_line_width(0.3*dev_->GetPitchX());
			mmContext->stroke();
		}
	}
	auto pattern = mmContext->pop_group();
	mmContext->set_source(pattern);
	mmContext->paint();
}


void TactileDisplayPainter::PaintSimple(const Cairo::RefPtr<Cairo::Context> &mmContext)
{
	mmContext->set_line_width(0.0);
	mmContext->set_source_rgba(1.0, 0.0, 0.0, 0.5);
	mmContext->rectangle(
		-dev_->GetWidth()/2.0,
		-dev_->GetHeight()/2.0,
		dev_->GetWidth(),
		dev_->GetHeight());
	mmContext->fill();
	mmContext->stroke();
}



} // namespace
