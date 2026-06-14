#include "tactiledisplaypainter.h"
#include "point.h"

namespace latero::graphics {

TactileDisplayPainter::TactileDisplayPainter(const latero::TactileDisplay *dev) : dev_(dev)
{
}

void TactileDisplayPainter::Paint(const Cairo::RefPtr<Cairo::Context> &mmContext, const latero::BiasedImg &tdState, bool drawOutline)
{
	mmContext->push_group();
	double tdw = dev_->GetWidth()*1.4;
	double tdh = dev_->GetHeight()*1.2;

	if (drawOutline)
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
			latero::graphics::Point p = dev_->GetActuatorOffset(i,j);
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

} // namespace
