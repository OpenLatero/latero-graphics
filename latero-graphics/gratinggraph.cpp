// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#include "gratinggraph.h"
#include "grating.h"
#include <math.h>

namespace latero {
namespace graphics { 

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

} // namespace graphics
} // namespace latero
