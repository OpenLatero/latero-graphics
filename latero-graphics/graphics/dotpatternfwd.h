#pragma once

#include <boost/shared_ptr.hpp>

namespace latero::graphics {

class DotPattern;
typedef boost::shared_ptr<DotPattern> DotPatternPtr;

class DottedLine;
typedef boost::shared_ptr<DottedLine> DottedLinePtr;

class DottedCircle;
typedef boost::shared_ptr<DottedCircle> DottedCirclePtr;

class DottedPolygon;
typedef boost::shared_ptr<DottedPolygon> DottedPolygonPtr;


} // namespace

