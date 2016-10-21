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

#include "ridgegraph.h"
#include "ridge.h"

namespace latero {
namespace graphics { 

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

} // namespace graphics
} // namespace latero
