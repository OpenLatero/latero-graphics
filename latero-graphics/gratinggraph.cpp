#include "gratinggraph.h"
#include "grating.h"
#include <math.h>

namespace latero::graphics {

GratingGraph::GratingGraph(GratingPtr peer) :
	peer_(peer)
{	
	plot_.set_size_request(200, 200);
	plot_.SetRangeX(0, peer->GetRhythmSize());
	plot_.SetRangeY(0.0, 1.0);

	// TODO: turn auto-refresh into an optional feature of Graph
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &GratingGraph::OnCheckPeer),
		(uint)333, // ms
		Glib::PRIORITY_DEFAULT_IDLE);
}

GratingGraph::~GratingGraph()
{
}

double GratingGraph::GetValue(double x)
{
	if (!peer_) return 0;
	return peer_->Render_(x,boost::posix_time::seconds(0)); // todo: not safe!
}

bool GratingGraph::OnCheckPeer()
{
	if (!peer_) return true;
	if (peer_->GetLastModified() > refreshTime_)
	{
		plot_.SetRangeX(0, peer_->GetRhythmSize());
		Refresh();
	}
	return true;
}

} // namespace
