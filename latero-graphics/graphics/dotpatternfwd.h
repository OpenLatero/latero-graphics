#pragma once

#include <memory>

namespace latero::graphics {

class DotPattern;
typedef std::shared_ptr<DotPattern> DotPatternPtr;

class DottedLine;
typedef std::shared_ptr<DottedLine> DottedLinePtr;

class DottedCircle;
typedef std::shared_ptr<DottedCircle> DottedCirclePtr;

class DottedPolygon;
typedef std::shared_ptr<DottedPolygon> DottedPolygonPtr;


} // namespace

