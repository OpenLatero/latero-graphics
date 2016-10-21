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

#ifndef LATERO_GRAPHICS_PLANAR_POLYGON
#define LATERO_GRAPHICS_PLANAR_POLYGON

#include "polygonfwd.h"
#include "closedpattern.h"
#include "stroke.h"
#include "../vector.h"
#include "../point.h"
#include "../oscillatorfwd.h"
#include "../units.h"
#include <iostream>
#include "dotpattern.h"


namespace latero {
namespace graphics { 

class Vertex
{
public:
	Vertex() {};
	virtual ~Vertex() {};

	Point pos;		// location of the vertex
	Point contact;		// location of contact between the strokes
	double angle;		// smallest angle at this joint
	double intersectDist;	// distance of intersection from the node

	Vector to_prev, to_prev_unit;		// vector towards previous vertex (unit vector or full length)
	Vector to_next, to_next_unit;		// vector towards next vertex (unit vector or full length)
	Vector to_contact, to_contact_unit;

	Point p_prev_intersect;	// intersection point along the line towards the previous vertex
	Point p_next_intersect;  // intersection point along the line towards the next vertex

	Point p_prev_circle_intercept; // interception point where the circle meets the edge
	Point p_next_circle_intercept;

	double rounded_corner_length; // length of this corner, if rounded (join_type_rotate)

	void Dump()
	{
		printf("---------------------\n");
		printf("pos: %f,%f\n", pos.x, pos.y);
	}

	int id; 

};

class Edge
{
public:
	Edge() : id(-1) {};

	Edge(int p_id, Vertex &v1, Vertex &v2) :
		id(p_id), startPos(v1.pos), endPos(v2.pos)
	{
		length = (endPos - startPos).Norm();

		rounded_length = Length();
		rounded_length -= v1.intersectDist + v2.intersectDist;
		rounded_length += 0.5 * (v1.rounded_corner_length + v2.rounded_corner_length);
	};
	virtual ~Edge() {};

	inline Point &StartPos() { return startPos; }
	inline Point &EndPos() { return endPos; }

	inline double Length() const { return length; }
	inline double RoundedLength() const { return rounded_length; }

	int id;

protected:

	Point startPos, endPos;
	double length;		// length from vertex to vertex
	double rounded_length;	// length assuming rounded corners (join_type_rotate)
};


class Corner
{
public:
	Corner() {};
	virtual ~Corner() {};

	Point vertex;
	Point p_limit_0;
	Vector v_limit_1,v_limit_2;
	Vector n_limit_1,n_limit_2;
};



class Polygon : public ClosedPattern
{
public:
	typedef NamedID JoinType;
	typedef NamedIDSet JoinTypeSet;
	static const JoinType join_type_straight, join_type_round;

	typedef NamedID CornerSpan;
	typedef NamedIDSet CornerSpanSet;
	static const CornerSpan corner_span_user, corner_span_arc, corner_span_circle, corner_span_right;

	typedef NamedID CornerType;
	typedef NamedIDSet CornerTypeSet;
	static const CornerType corner_type_solid, corner_type_invert_vib;

	typedef NamedID CornerBlend;
	typedef NamedIDSet CornerBlendSet;
	static const CornerBlend corner_blend_in, corner_blend_out;

	static PolygonPtr Create(const latero::Tactograph *dev)
	{
		return PolygonPtr(new Polygon(dev));
	}

	static PolygonPtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return PolygonPtr(new Polygon(dev, node));
	}

	virtual ~Polygon() {};
	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t);

	void LoadDefaults();
	virtual void AppendXML(XMLOutputNode &root) const;
	virtual void LoadXML(const XMLInputNode &root);

	/** 
	 * Compute the longitudinal (along the length) and latitudinal (along the width) distances
	 * at a given point. 
	 * @return closest edge
	 */
	Edge GetDistances_(const Point &pos, double &longitudinal, double &latitudinal);
	double GetLatitudinalDistance_(const Point &pos);
	virtual std::string GetName() { return "Polygon"; };

	void InsertPoint(const Point &p);
	std::vector<Point> GetPoints() const { LATERO_GRAPHICS_GUARD; return points_; }
	void SetPoints(const std::vector<Point> &p);

	StrokePtr GetStroke() 		{ return stroke_; }
	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

	JoinTypeSet GetJoinTypes() const { return joinTypeSet_; };
	JoinType GetJoinType() const { LATERO_GRAPHICS_GUARD; return joinType_; }
	void SetJoinType(const JoinType &mode)
	{
		{
			LATERO_GRAPHICS_GUARD;
			joinType_ = mode;
			SetLastModified_();
		}
		Precompute();
	}
	void SetJoinType(std::string label);

	void SetRoundingOffset(double v)
	{
		{
			LATERO_GRAPHICS_GUARD; 
			roundingOffset_ = v;
			SetLastModified_();
		}
		Precompute();
	}
	double GetRoundingOffset() const { LATERO_GRAPHICS_GUARD; return roundingOffset_; }

	/***********************************
	 * Joint Markers
	 ***********************************/

	void SetCornerEnable(bool v)
	{
		{
			LATERO_GRAPHICS_GUARD;
			cornerEnable_=v;
			SetLastModified_();
		}
		dots_->SetInvertCornerVib(v);
		Precompute();
	}
	bool GetCornerEnable() const { return cornerEnable_; }

	void SetCornerMotion(bool v) { LATERO_GRAPHICS_GUARD; cornerMotion_=v; SetLastModified_(); }
	bool GetCornerMotion() const { return cornerMotion_; }

	CornerSpanSet GetCornerSpans() const { return cornerSpans_; };
	CornerSpan GetCornerSpan() const { LATERO_GRAPHICS_GUARD; return cornerSpan_; }
	void SetCornerSpan(const CornerSpan &mode)
	{
		{
			LATERO_GRAPHICS_GUARD;
			cornerSpan_ = mode;
			SetLastModified_();
		}
		Precompute();
	}
	void SetCornerSpan(std::string label);

	void SetCornerUserAngle(double degree)
	{
		{
			LATERO_GRAPHICS_GUARD; 
			cornerUserAngle_ = units::DegreeToRad(degree); 
			SetLastModified_(); 
		}
		Precompute();
	}
	double GetCornerUserAngle() const
		{ LATERO_GRAPHICS_GUARD; return units::RadToDegree(cornerUserAngle_); }

	void SetCornerBlendSize(double v)
	{
		{
			LATERO_GRAPHICS_GUARD;
			cornerBlendSize_ = v;
			SetLastModified_();
		}
		Precompute();
	}
	double GetCornerBlendSize() const
		{ LATERO_GRAPHICS_GUARD; return cornerBlendSize_; }

	CornerBlendSet GetCornerBlends() const { return cornerBlends_; };
	CornerBlend GetCornerBlend() const { LATERO_GRAPHICS_GUARD; return cornerBlend_; }
	void SetCornerBlend(const CornerBlend &mode)
	{
		{
			LATERO_GRAPHICS_GUARD;
			cornerBlend_ = mode;
			SetLastModified_();
		}
		Precompute();
	}
	void SetCornerBlend(std::string label);

	CornerTypeSet GetCornerTypes() const { return cornerTypes_; };
	CornerType GetCornerType() const { LATERO_GRAPHICS_GUARD; return cornerType_; }
	void SetCornerType(const CornerType &mode)
	{
		{
			LATERO_GRAPHICS_GUARD;
			cornerType_ = mode;
			SetLastModified_();
		}
		Precompute();
	}
	void SetCornerType(std::string label);

	OscillatorPtr GetCornerOscillator() { return cornerVib_; }
	virtual bool UpdateVisible_(const State *state);

private:
	double RenderFilling_(double dLength, double dWidth, const ActuatorState &state);

	double RenderCorner_(const StrokeState &state, const Point &pos, Edge &edge);
	std::vector<Vertex> GetVertices() const { LATERO_GRAPHICS_GUARD; return largeVertex_; }

	Edge GetClosestEdge_(const Point &pos, double &dLength, double &dWidth);
	Corner GetClosestCorner_(const Point &pos);

	Polygon(const latero::Tactograph *dev);
	Polygon(const latero::Tactograph *dev, const XMLInputNode &node);

	void Precompute();
	std::vector<Vertex> PrecomputeVertices(std::vector<Point> &points, double strokeRadius) const;
	std::vector<Edge> PrecomputeEdges(std::vector<Vertex> &vertices) const;
	std::vector<Corner> PrecomputeCorners(const std::vector<Vertex> &vertices) const;
	double PrecomputeLength(const std::vector<Edge> &edges) const;

	void InsertPoint_(const Point &p);

	std::vector<Point> points_;

	StrokePtr stroke_;

	JoinType joinType_;
	JoinTypeSet joinTypeSet_;
	double roundingOffset_; // offset to line thikness used in join_type_rotate mode

	bool cornerEnable_;
	CornerSpan cornerSpan_;
	CornerSpanSet cornerSpans_;
	CornerBlend cornerBlend_;
	double cornerBlendSize_;
	CornerBlendSet cornerBlends_;
	CornerType cornerType_;
	CornerTypeSet cornerTypes_;
	double cornerUserAngle_; // when in "user" mode
	OscillatorPtr cornerVib_;	
	bool cornerMotion_;

	// precomputed stuff
	std::vector<Vertex> vertex_; // does NOT takes into account oversized motion cues and roundingOffset
	std::vector<Vertex> largeVertex_; // takes into account oversized motion cues and roundingOffset
	std::vector<Edge> edges_;
	std::vector<Corner> corners_;

	DottedPolygonPtr dots_;	// dots rendering
};

} // namespace graphics
} // namespace latero

#endif
