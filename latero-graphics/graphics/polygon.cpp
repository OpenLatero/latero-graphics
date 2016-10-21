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

#include "polygon.h"
#include <math.h>
#include "polygonwidget.h"
#include "../vector.h"
#include "../grating.h"
#include "../ridge.h"
#include "../oscillator.h"
#include "texture/texture.h"
#include "texture/patternops.h"
#include "tactileops.h"


namespace latero {
namespace graphics { 

const Polygon::JoinType Polygon::join_type_straight(0,"straight");
const Polygon::JoinType Polygon::join_type_round(1,"round");

const Polygon::CornerSpan Polygon::corner_span_user(0,"user specified");
const Polygon::CornerSpan Polygon::corner_span_arc(1,"arc");
const Polygon::CornerSpan Polygon::corner_span_circle(2,"circle intercept");
const Polygon::CornerSpan Polygon::corner_span_right(3,"right-angled");

const Polygon::CornerBlend Polygon::corner_blend_in(0,"in");
const Polygon::CornerBlend Polygon::corner_blend_out(1,"out");

const Polygon::CornerType Polygon::corner_type_solid(0,"solid");
const Polygon::CornerType Polygon::corner_type_invert_vib(1,"invert vib");



Polygon::Polygon(const latero::Tactograph *dev, const XMLInputNode &node) :	ClosedPattern(dev)
{
	LoadDefaults();
	LoadXML(node);
}

Polygon::Polygon(const latero::Tactograph *dev) : ClosedPattern(dev)
{
	LoadDefaults();
}

void Polygon::LoadDefaults()
{
	cornerBlendSize_ = 1;

	dots_ = DottedPolygon::Create(Dev());
	stroke_ = Stroke::Create(Dev());
	stroke_->SetDots(dots_);
	cornerUserAngle_ = units::DegreeToRad(30);
	roundingOffset_ = 0;
	cornerVib_ = Oscillator::Create(50,1,true);
	cornerEnable_ = false;
	cornerMotion_ = true;
	cornerType_ = corner_type_solid;
 	joinType_ = join_type_round;
	cornerSpan_ = corner_span_circle;
	cornerBlend_ = corner_blend_out;

	joinTypeSet_.Append(join_type_straight);
	joinTypeSet_.Append(join_type_round);

	cornerSpans_.Append(corner_span_user);
	cornerSpans_.Append(corner_span_arc);
	cornerSpans_.Append(corner_span_circle);
	cornerSpans_.Append(corner_span_right);

	cornerBlends_.Append(corner_blend_in);
	cornerBlends_.Append(corner_blend_out);

	cornerTypes_.Append(corner_type_solid);
	cornerTypes_.Append(corner_type_invert_vib);

	stroke_->signal_width_changed().connect(sigc::mem_fun(*this, &Polygon::Precompute));

	AddModifiableChild(stroke_);
	AddModifiableChild(cornerVib_);
}


void Polygon::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode polynode = root.AddChild("Pattern");
	polynode.SetType("Polygon");

	XMLOutputNode node = polynode.AddChild("vertices");
	std::vector<Point> list = GetPoints();
	for (uint i=0; i<list.size(); ++i)
		node.AddChild("vertex", list[i], units::mm);

	XMLOutputNode joinNode = polynode.AddChild("join");
	joinNode.AddChild("type", joinType_.label);
	joinNode.AddChild("rounding_offset", roundingOffset_, units::mm);

	XMLOutputNode cornerNode = polynode.AddChild("corner");
	cornerNode.AddChild("enable", cornerEnable_);
	cornerNode.AddChild("span", cornerSpan_.label);
	cornerNode.AddChild("blend", cornerBlend_.label);
	cornerNode.AddChild("blend_size", GetCornerBlendSize(), units::mm);
	cornerNode.AddChild("type", cornerType_.label);
	cornerNode.AddChild("user_angle", GetCornerUserAngle(), units::degree);
	cornerNode.AddChild("motion", cornerMotion_);
	cornerVib_->AppendXML(cornerNode);

	stroke_->AppendXML(polynode);
	ClosedPattern::AppendXML(polynode);
}

void Polygon::LoadXML(const XMLInputNode &root)
{
	ClosedPattern::LoadXML(root);
	stroke_->LoadXML(root);

	XMLInputNode vertexNode = root.GetChild("vertices");
	if (vertexNode)
	{
		std::vector<XMLInputNode> list = vertexNode.GetChildren("vertex");
		std::vector<Point> points;
		for(unsigned int i=0; i<list.size(); ++i)
			points.push_back(list[i].GetPoint());
		SetPoints(points);
	}


	XMLInputNode joinNode = root.GetChild("join");
	if (joinNode)
	{
		XMLInputNode node = joinNode.GetChild("type");
		if (node) SetJoinType(node.GetString());

		node = joinNode.GetChild("rounding_offset");
		if (node) SetRoundingOffset(node.GetFloat());
	}

	XMLInputNode cornerNode = root.GetChild("corner");
	if (!cornerNode)
		SetCornerEnable(false);
	else
	{
		XMLInputNode node = cornerNode.GetChild("enable");
		if (node) SetCornerEnable(node.GetBool());

		node = cornerNode.GetChild("span");
		if (node) SetCornerSpan(node.GetString());

		node = cornerNode.GetChild("blend");
		if (node) SetCornerBlend(node.GetString());

		node = cornerNode.GetChild("blend_size");
		if (node) SetCornerBlendSize(node.GetFloat());

		node = cornerNode.GetChild("type");
		if (node) SetCornerType(node.GetString());

		node = cornerNode.GetChild("user_angle");
		if (node) SetCornerUserAngle(node.GetFloat());

		node = cornerNode.GetChild("motion");
		if (node) SetCornerMotion(node.GetBool());

		cornerVib_->LoadXML(cornerNode);
	}
}

Cairo::RefPtr<Cairo::Pattern> Polygon::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t)
{
	cr->push_group();

	std::vector<Vertex> vertex = GetVertices(); // NOT SAFE, can't use Next(), Prev();
	if (vertex.size() < 3) return cr->pop_group();

	double value = stroke_->GetIntensity();
	double thickness = stroke_->GetWidth();

	if (GetFillTextureEnable())
	{
		cr->push_group();
		cr->move_to(vertex[0].pos.x, vertex[0].pos.y);
		for (unsigned int i=1; i<vertex.size(); ++i)
			cr->line_to(vertex[i].pos.x, vertex[i].pos.y);
		cr->close_path();
		cr->set_line_cap(Cairo::LINE_CAP_ROUND);
		cr->set_line_join(Cairo::LINE_JOIN_ROUND);

		cr->set_line_width(thickness+2*GetFillTextureGap());
		//cr->set_source_rgba(0,0,0,1);
		//cr->stroke_preserve(); // todo: doesn't seem to work anymore (Ubuntu 10.10)

		cr->set_source_rgba(0,0,0,value);
		cr->set_operator(Cairo::OPERATOR_OUT);
		cr->fill();

		Cairo::RefPtr<Cairo::Pattern> fillMask = cr->pop_group();

		Cairo::RefPtr<Cairo::Pattern> txMask = GetFillTexture()->GetDrawingPattern(cr,t);

		Cairo::RefPtr<Cairo::Pattern> fill = PatternOps::Multiply(cr, fillMask, txMask);
		cr->set_source(fill);
		cr->paint();
	}

	if (stroke_->GetDotEnable())
	{
		cr->set_source(dots_->GetDrawingPattern(cr,t));
		cr->paint();
	}
	else
	{
		cr->push_group();
		cr->set_source_rgba(0,0,0,value);
		cr->set_line_width(thickness);
		cr->move_to(vertex[0].pos.x, vertex[0].pos.y);
		for (unsigned int i=1; i<vertex.size(); ++i)
			cr->line_to(vertex[i].pos.x, vertex[i].pos.y);
		cr->close_path();
		cr->set_line_cap(Cairo::LINE_CAP_ROUND);
		cr->set_line_join(Cairo::LINE_JOIN_ROUND);
		cr->stroke();
		Cairo::RefPtr<Cairo::Pattern> strokePattern = cr->pop_group();

		if (stroke_->GetOscillator()->GetEnable())
			strokePattern = stroke_->GetOscillator()->GetBlendPattern(strokePattern,cr);
		cr->set_source(strokePattern);
		cr->paint();
	}
	return cr->pop_group();
}


Gtk::Widget *Polygon::CreateWidget(PatternPtr ptr)
{
	PolygonPtr gen = boost::dynamic_pointer_cast<Polygon>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new PolygonWidget(gen);
}





double Polygon::RenderFilling_(double dLength, double dWidth, const ActuatorState &state)
{
	if (!GetFillTextureEnable_() || !GetFillTexture_() || !IsInsidePolygon(state.pos, points_))
	{
		// texture is disabled or doesn't exist, or outside the shape
		return 0;
	}
	else
	{
		// inside shape
		double dFromEdge = dWidth - stroke_->GetWidth_()/2;
		return ClosedPattern::RenderFilling_(dFromEdge, state);
	}
}


double Polygon::DoRender_(const ActuatorState &state)
{
	StrokeState strokeState(state);
	Edge edge = GetDistances_(state.pos, strokeState.dLength, strokeState.dWidth);

	double filling = RenderFilling_(strokeState.dLength,strokeState.dWidth,state);

	if (stroke_->GetDotEnable_() || !cornerEnable_)
	{
		return stroke_->Render_(strokeState, filling);
	}
	else
	{
		// alpha value of the corner (along the length of the stroke only)
		double cornerAlpha = RenderCorner_(strokeState, state.pos, edge);
		double motionCueAlpha = (cornerMotion_)?1:1-cornerAlpha;
		double invertVib = (cornerType_ == corner_type_invert_vib)?cornerAlpha:0;
		double overlayAlpha = (cornerType_ == corner_type_solid) ? cornerAlpha : 0;
		double overlayValue = (cornerType_ == corner_type_solid) ? cornerVib_->RenderBlend_(state) : 0;
		return stroke_->Render_(strokeState, filling, motionCueAlpha, invertVib, overlayValue, overlayAlpha);
	}

}

/**
 * @param strokeBlend blending ratio between corner and normal stroke (0=stroke, 1=corner)
 */
double Polygon::RenderCorner_(const StrokeState &state, const Point &pos, Edge &edge)
{
	if (!cornerEnable_) return 0;
	if (state.dWidth > stroke_->GetWidth_()/2) return 0;

	Corner corner = GetClosestCorner_(pos);

	// Check if the edge we're in is actually connected to the closest vertex. If we don't 
	// do this, the corner can appear in the middle of another edge to which a vertex is 
	// close to.
	if ((corner.vertex != edge.StartPos()) && !(corner.vertex == edge.EndPos()))
		return 0;


	// Check we're inside the two halfplanes defining the limits of the corner
	bool inLimit1 = DotProduct(pos - corner.p_limit_0, corner.n_limit_1)<0;
	bool inLimit2 = DotProduct(pos - corner.p_limit_0, corner.n_limit_2)>0;
	if (inLimit1 && inLimit2)
	{	
		// pick the minimal distance to the limits
		double dLimit1 = GetDistanceToInfiniteLine(pos, corner.p_limit_0, corner.v_limit_1);
		double dLimit2 = GetDistanceToInfiniteLine(pos, corner.p_limit_0, corner.v_limit_2);
		return fmin(1,fmin(dLimit1,dLimit2)/cornerBlendSize_);
	}

	return 0;
}



double Polygon::GetLatitudinalDistance_(const Point &pos)
{
	int n = largeVertex_.size();
	if (n < 3) return 0;

	double dWidth = 10000000;
	for (unsigned int i=0; i<edges_.size(); ++i)
	{
		double d = GetDistanceToLineSegment(pos, edges_[i].StartPos(), edges_[i].EndPos());
		if (d<dWidth)
			dWidth = d;
	}

	return dWidth;
}


bool Polygon::UpdateVisible_(const State *state)
{
	double d = GetLatitudinalDistance_(state->pos);
	bool visible = (d <= (stroke_->GetEffectiveWidth_()/2 + Dev()->GetRadius()));
	if (!visible && GetFillTextureEnable_())
		visible = IsInsidePolygon(state->pos, points_);
	SetVisible_(visible);
	return visible;
}


double Polygon::DoRenderShadow_(const ActuatorState &state)
{
	if (GetFillTextureEnable_())
		if (IsInsidePolygon(state.pos, points_))
			return 1;
	return stroke_->RenderShadow_(GetLatitudinalDistance_(state.pos));
}



Corner Polygon::GetClosestCorner_(const Point &pos)
{
	int rv = 0;
	double d2 = 10000000;
	for (unsigned int i=0; i<largeVertex_.size(); ++i)
	{
		double dsquare = (pos - largeVertex_[i].pos).NormSquared();
		if (dsquare < d2)
		{
			rv = i;
			d2 = dsquare;
		}
	}
	return corners_[rv];
}

Edge Polygon::GetClosestEdge_(const Point &pos, double &dLength, double &dWidth)
{
	int line = 0;
	dWidth = 10000000;
	dLength = 10000000;
	for (unsigned int i=0; i<edges_.size(); ++i)
	{
		double d,l;
		GetOrthoDistancesToLineSegment(pos, edges_[i].StartPos(), edges_[i].EndPos(), l, d);
		if (fabs(d-dWidth) < 0.00001)
		{
			// TODO: This is necessary only when corners are not rounded.

			// This point is in a part of the corner where it is closest to the vertex, and hence
			// at equal distance from two edges. 
		
			// Compute the shortest distance to the infinite line formed by the current edge. This is
			// equivalent to the longitudinal distance along the adjacent edge with respect to the shared
			// vertex.
			double dCurEdge = (dLength<0) ? fabs(dLength) : dLength - edges_[line].Length();

			// Repeat the computation for the distance to the adjacent edge.
			double dPrevEdge = (l<0) ? fabs(l) : l - edges_[i].Length();

			// The trick here is that the length "l" is negative if the shared vertex is before the edge
			// and positive with an offset equal to the length of the edge otherwise. In most cases the
			// current edge shares a vertex with the previous edge but we also have to consider the case
			// where the last edge shares the vertex of the first edge, and hence roles are reversed,

			// If we're closer to the other edge, it means that we're on the side of the current edge.
			if (dCurEdge > dPrevEdge)
			{
				line = i;
				dWidth = d;
				dLength = l;
			}
		}
		else if (d<dWidth)
		{
			line = i;
			dWidth = d;
			dLength = l;
		}
	}
	return edges_[line];
}

Edge Polygon::GetDistances_(const Point &pos, double &dLength, double &dWidth)
{
	int n = largeVertex_.size();
	if (n < 3)
	{
		dLength = dWidth = 0;
		return Edge();
	}

	Edge edge = GetClosestEdge_(pos, dLength, dWidth);

	if (joinType_ == join_type_round)
	{
		Vertex start = largeVertex_[edge.id];
		Vertex end = largeVertex_[(edge.id+1)%largeVertex_.size()];
		if (dLength < start.intersectDist)
		{
			Vector v1 = start.p_next_intersect - start.contact;
			Vector v2 = pos - start.contact;
			double theta = AngleRad(v1,v2);
			double span = M_PI - start.angle;
			dLength = 0.5*start.rounded_corner_length  - (theta/span) *start.rounded_corner_length;
		}
		else if (dLength > edge.Length() - end.intersectDist)
		{
			Vector v1 = end.p_prev_intersect - end.contact;
			Vector v2 = pos - end.contact;
			double theta = AngleRad(v1,v2);
			double span = M_PI - end.angle;
			dLength = edge.Length();
			dLength = dLength - start.intersectDist + 0.5*start.rounded_corner_length;
			dLength = dLength - end.intersectDist + (theta/span) * end.rounded_corner_length;
		}
		else
		{
			dLength = dLength - start.intersectDist + 0.5*start.rounded_corner_length;
		}

		for (int i=0; i<edge.id; ++i)
			dLength += edges_[i].RoundedLength();
	}
	else
	{
		for (int i=0; i<edge.id; ++i)
			dLength += edges_[i].Length();
	}
	return edge;
}




void Polygon::SetJoinType(std::string label)
{
	JoinType *op = joinTypeSet_.Get(label);
	if (op) SetJoinType(*op);
}

void Polygon::SetCornerSpan(std::string label)
{
	CornerSpan *op = cornerSpans_.Get(label);
	if (op) SetCornerSpan(*op);
}

void Polygon::SetCornerBlend(std::string label)
{
	CornerBlend *op = cornerBlends_.Get(label);
	if (op) SetCornerBlend(*op);
}

void Polygon::SetCornerType(std::string label)
{
	CornerType *op = cornerTypes_.Get(label);
	if (op) SetCornerType(*op);
}

void Polygon::Precompute()
{
	// make most computations without locking...
	std::vector<Point> points = GetPoints();
	std::vector<Vertex> vertex = PrecomputeVertices(points, stroke_->GetWidth()/2);
	std::vector<Vertex> largeVertex = PrecomputeVertices(points, GetRoundingOffset() + stroke_->GetEffectiveWidth()/2);
	std::vector<Edge> edges = PrecomputeEdges(largeVertex);
	std::vector<Corner> corners = PrecomputeCorners(vertex);
	double d = PrecomputeLength(edges);

	LATERO_GRAPHICS_GUARD;
	vertex_ = vertex;
	largeVertex_ = largeVertex;
	edges_ = edges;
	corners_ = corners;
	stroke_->SetLength(d);
};


double Polygon::PrecomputeLength(const std::vector<Edge> &edges) const
{
	// recompute the sum of lengths
	JoinType joinType = GetJoinType();
	double d = 0;
	for (unsigned int i=0; i<edges.size(); ++i)
	{
		if (joinType == join_type_round)
			d+=edges[i].RoundedLength();
		else
			d+=edges[i].Length();
	}
	return d;
}

std::vector<Vertex> Polygon::PrecomputeVertices(std::vector<Point> &points, double strokeRadius) const
{
	int n = points.size();
	std::vector<Vertex> vertices;
	if (n<3) return vertices;

	for (int i=0; i<n; ++i)
	{
		Vertex v;
		v.id = i;
		v.pos = points[i];
		vertices.push_back(v);
	}

	/**
	 * next and previous
	 */
	for (int i=0; i<n; ++i)
	{
		Vertex* prev = &vertices[(i+n-1)%n];
		Vertex* next = &vertices[(i+1)%n];
		vertices[i].to_prev = prev->pos - vertices[i].pos;
		vertices[i].to_prev_unit = vertices[i].to_prev.Unit();
		vertices[i].to_next = next->pos - vertices[i].pos;
		vertices[i].to_next_unit = vertices[i].to_next.Unit();
	}

	/**
	 * Find the angle of each vertex. Since we don't know which way the polygon will close, this
	 * angle is simply a number between 0 and Pi and doesn't indicate the direction of the bend.
	 */
	for (int i=0; i<n; ++i)
		vertices[i].angle = AngleRad(vertices[i].to_prev, vertices[i].to_next);

	/**
	 * Compute the distance along the line between the vertex and the location where the strokes
	 * intersect.
	 */
	for (int i=0; i<n; ++i)
		vertices[i].intersectDist  = strokeRadius / tan(vertices[i].angle/2);

	/**
	 * Compute the points where the stokes intersect.
	 */
	for (int i=0; i<n; ++i)
	{
		Vector dir = vertices[i].to_prev_unit + vertices[i].to_next_unit;
		dir = dir.Unit();
		double l = sqrt(vertices[i].intersectDist*vertices[i].intersectDist + strokeRadius*strokeRadius);
		vertices[i].contact = vertices[i].pos + dir*l;
		vertices[i].to_contact = vertices[i].contact - vertices[i].pos;
		vertices[i].to_contact_unit = vertices[i].to_contact.Unit();
	}

	/**
	 * Compute the intersection points around the vertex.
	 */
	for (int i=0; i<n; ++i)
	{
		vertices[i].p_prev_intersect = vertices[i].pos + vertices[i].to_prev_unit*vertices[i].intersectDist;
		vertices[i].p_next_intersect = vertices[i].pos + vertices[i].to_next_unit*vertices[i].intersectDist;
	}

	/**
	 * Length of corners when rounded.
	 */
	for (int i=0; i<n; ++i)
		vertices[i].rounded_corner_length = (M_PI-vertices[i].angle)*strokeRadius;

	/**
	 * Interception points
	 */
	for (int i=0; i<n; ++i)
	{
		// TODO: this will use the enlarged radius when motion cues are enlarged...
		Vector pdir = vertices[i].p_prev_intersect - vertices[i].contact;
		vertices[i].p_prev_circle_intercept = vertices[i].pos + pdir.Unit()*strokeRadius;

		Vector ndir = vertices[i].p_next_intersect - vertices[i].contact;
		vertices[i].p_next_circle_intercept = vertices[i].pos + ndir.Unit()*strokeRadius;
	}

	return vertices;
}


std::vector<Edge> Polygon::PrecomputeEdges(std::vector<Vertex> &vertices) const
{
	std::vector<Edge> edges;
	int n = vertices.size();
	for (int i=0; i<n; ++i)
		edges.push_back(Edge(i, vertices[i], vertices[(i+1)%n]));
	return edges;
}

std::vector<Corner> Polygon::PrecomputeCorners(const std::vector<Vertex> &vertices) const
{
	std::vector<Corner> corners;
	int n = vertices.size();

	double cornerUserAngle = GetCornerUserAngle();
	CornerSpan cornerSpan = GetCornerSpan();
	double cornerBlendSize = GetCornerBlendSize();
	CornerBlend cornerBlend = GetCornerBlend();

	for (int i=0; i<n; ++i)
	{
		Corner corner;

		Point p0 = vertices[i].pos;
		Point p1 = vertices[i].contact;
		Point p2 = vertices[i].p_prev_circle_intercept;
		Point p3 = vertices[i].p_next_circle_intercept;

		Vector v10 = p0 - p1;
		Vector v12 = p2 - p1;
		Vector v13 = p3 - p1;

		double theta = vertices[i].angle;
		double a1 = AngleRad(v12, v13);
		double a2 = theta;
		double a3 = M_PI - theta;

		double size = fmin(cornerUserAngle, fmax(a2,a3));
		if (cornerSpan == corner_span_arc)		size = a1;
		else if (cornerSpan == corner_span_circle)	size = a2;
		else if (cornerSpan == corner_span_right)	size = a3;
		
		bool concave = true;
		Vector u14 = v10.Rotated(+size/2).Unit();
		Vector u15 = v10.Rotated(-size/2).Unit();
		Vector diff = u15-u14;
		if (DotProduct(diff, vertices[i].to_next) < 0)
		{
			concave = false;
			Vector tmp = u14;
			u14 = u15;
			u15 = tmp;
		}

		corner.vertex = p0;

		// normals, towards positive angle
		Vector n14 = Point(u14.y, - u14.x);
		if (concave) n14 *= -1;
		Vector n15 = Point(u15.y, - u15.x);
		if (concave) n15 *= -1;

		Vector u_to_prev = vertices[i].to_prev_unit;
		Vector u_to_next = vertices[i].to_next_unit;

		corner.n_limit_1 = n14;
		corner.n_limit_2 = n15;
		corner.v_limit_1 = u14;
		corner.v_limit_2 = u15;
		corner.p_limit_0 = p1;
		if (cornerBlend == corner_blend_out)
		{
			double offset = cornerBlendSize;
			Point p_limit_1 = p1  + u_to_prev*offset;
			Point p_limit_2 = p1  + u_to_next*offset;
			corner.p_limit_0 = GetIntersectionOfLines(p_limit_1, corner.v_limit_1, p_limit_2, corner.v_limit_2);
		}

		corners.push_back(corner);
	}
	return corners;
}


void Polygon::SetPoints(const std::vector<Point> &p)
{
	LATERO_GRAPHICS_LOCK;
	points_=p;
	LATERO_GRAPHICS_UNLOCK;
	dots_->SetVertices(p);
	Precompute();
	SetLastModified();
}

void Polygon::InsertPoint(const Point &p)
{
	LATERO_GRAPHICS_LOCK;
	points_.push_back(p);
	LATERO_GRAPHICS_UNLOCK;
	dots_->SetVertices(points_);
	Precompute();
	SetLastModified();
}

} // namespace graphics
} // namespace latero



