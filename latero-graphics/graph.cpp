#include "graph.h"

#define NB_POINTS 1000

namespace latero::graphics {

Graph::Graph(uint nbPoints, const char *fgColor, const char *bgColor) :
	refreshTime_(boost::posix_time::min_date_time),
	nbPoints_(nbPoints),
	plot_(fgColor, bgColor)
{	
	append(plot_);
	plot_.set_vexpand();
	plot_.set_hexpand();
}

Graph::~Graph()
{
}

void Graph::Refresh()
{
	UpdateRange();
	refreshTime_ = boost::posix_time::microsec_clock::universal_time();

	plot_.Clear();
	float xMin = plot_.GetMinX();
	float xMax = plot_.GetMaxX();

	float incr = (xMax-xMin) / nbPoints_;
	for (float x=xMin; x<xMax; x+=incr)
	{
		plot_.InsertPoint(x, GetValue(x));
	}
	plot_.Draw();
}

} // namespace
