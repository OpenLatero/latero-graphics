#pragma once

#include "ridge.h"
#include "graph.h"

namespace latero::graphics {

class RidgeGraph : public Graph
{
public:
	RidgeGraph(RidgePtr peer, int minWidth=-1, int minHeight=-1);
	virtual ~RidgeGraph();
	virtual void UpdateRange();
protected:
	bool OnCheckPeer();
	virtual double GetValue(double x);

	RidgePtr peer_;
};

} // namespace

