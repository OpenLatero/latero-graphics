#include "ridgegraph.h"
#include "ridge.h"

namespace latero::graphics {

RidgeGraph::RidgeGraph(RidgePtr peer, int minWidth, int minHeight) :
	peer_(peer)
{	
	set_size_request(minWidth,minHeight);
	//plot_.set_size_request(200, 200);

	// TODO: turn auto-refresh into an optional feature of Graph
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &RidgeGraph::OnCheckPeer),
		(uint)333, // ms
		Glib::PRIORITY_DEFAULT_IDLE);
}

void RidgeGraph::UpdateRange()
{
	// TODO: not a very clean way to handle this...
	plot_.SetRangeX(-peer_->GetWidth()/2, peer_->GetWidth()/2);
	plot_.SetRangeY(0.0, 1.0);
}

RidgeGraph::~RidgeGraph()
{
}

double RidgeGraph::GetValue(double x)
{
	return peer_->GetValue_(x);
}

bool RidgeGraph::OnCheckPeer()
{
	if (!peer_) return true;
	if (peer_->GetLastModified() > refreshTime_)
		Refresh();
	return true;
}

} // namespace
