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

#include "patternops.h"
#include <gtkmm.h>

namespace latero {
namespace graphics { 

Cairo::RefPtr<Cairo::Pattern> PatternOps::Add(Cairo::RefPtr<Cairo::Context> cr, Cairo::RefPtr<Cairo::Pattern> a, Cairo::RefPtr<Cairo::Pattern> b)
{
	cr->push_group();
	cr->set_source(a);
	cr->paint();
	cr->set_source(b);
	cr->set_operator(Cairo::OPERATOR_ADD);
	cr->paint();
	cr->set_operator(Cairo::OPERATOR_OVER);
	return cr->pop_group();
}

Cairo::RefPtr<Cairo::Pattern> PatternOps::Multiply(Cairo::RefPtr<Cairo::Context> cr, Cairo::RefPtr<Cairo::Pattern> a, Cairo::RefPtr<Cairo::Pattern> b)
{
	cr->push_group();
	cr->set_source(a);
	cr->paint();
	cr->set_source(b);
	cr->set_operator(Cairo::OPERATOR_IN);
	cr->paint();
	cr->set_operator(Cairo::OPERATOR_OVER);
	return cr->pop_group();
}

Cairo::RefPtr<Cairo::Pattern> PatternOps::Max(Cairo::RefPtr<Cairo::Context> cr, Cairo::RefPtr<Cairo::Pattern> a, Cairo::RefPtr<Cairo::Pattern> b)
{
	// TODO: It doesn't look like Cairomm supports this compositing operation. I'm not sure how it can be emulated. 
	// We most likely need to go through temporary image buffers.
	/*	
	double clip_x1, clip_y1, clip_x2, clip_y2;
	cr->get_clip_extents(clip_x1, clip_y1, clip_x2, clip_y2);
	
	double clip_pix_x1 = clip_x1;
	double clip_pix_y1 = clip_y1;
	double clip_pix_x2 = clip_x2;
	double clip_pix_y2 = clip_y2;
	cr->user_to_device(clip_pix_x1, clip_pix_y1); // get number of pixels
	cr->user_to_device(clip_pix_x2, clip_pix_y2);
	int pix_sx = clip_pix_x2 - clip_pix_x1;
	int pix_sy = clip_pix_y2 - clip_pix_y1;

	uint w = 1000;
	uint h = 1000;
	Glib::RefPtr<Gdk::Pixbuf> img_a = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, w, h);
	Cairo::RefPtr<Cairo::ImageSurface> surf_a = Cairo::ImageSurface::create(
		(unsigned char*)img_a->get_pixels(), Cairo::FORMAT_ARGB32, w, h, img_a->get_rowstride());
	Cairo::RefPtr<Cairo::Context> cr_a = Cairo::Context::create(surf_a);
	cr_a->scale(w/clip_x2, h/clip_y2);
	cr_a->set_source(a);
	cr_a->paint();
	img_a->save("test-a.png", "png");
	*/
	return Add(cr,a,b);
}

Cairo::RefPtr<Cairo::Pattern> PatternOps::Multiply(Cairo::RefPtr<Cairo::Context> cr, Cairo::RefPtr<Cairo::Pattern> a, double b)
{
	if (b==1) return a;
	cr->push_group();
	cr->set_source(a);
	cr->paint();
	cr->set_source_rgba(0,0,0,b);
	cr->set_operator(Cairo::OPERATOR_IN);
	cr->paint();
	cr->set_operator(Cairo::OPERATOR_OVER);
	return cr->pop_group();
}

Cairo::RefPtr<Cairo::Pattern> PatternOps::Invert(Cairo::RefPtr<Cairo::Context> cr, Cairo::RefPtr<Cairo::Pattern> a)
{
	cr->push_group();
	cr->set_source(a);
	cr->paint();
	cr->set_source_rgba(0,0,0,1.0);
	cr->set_operator(Cairo::OPERATOR_OUT);
	cr->paint();
	cr->set_operator(Cairo::OPERATOR_OVER);
	return cr->pop_group();
}

} // namespace graphics
} // namespace latero
