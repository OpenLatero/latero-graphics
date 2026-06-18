#pragma once

#include "grating.h"
#include "graph.h"

namespace latero::graphics {

class GratingGraph : public Graph
{
public:
	GratingGraph(GratingPtr peer);
	virtual ~GratingGraph();
protected:

	bool OnCheckPeer();
	virtual double GetValue(double x);

	GratingPtr peer_;
};

} // namespace

