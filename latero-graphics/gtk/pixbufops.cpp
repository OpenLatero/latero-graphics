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

#include "pixbufops.h"
#include <assert.h>

namespace latero {
namespace graphics { 

namespace gtk {

/*
void PixbufOps::CopyAlpha(Glib::RefPtr<Gdk::Pixbuf> &dest, const Glib::RefPtr<Gdk::Pixbuf> &src)
{
	assert(dest->get_height() == src->get_height());
	assert(dest->get_width() == src->get_width());
	assert(src->get_has_alpha());

	dest = dest->add_alpha(false,0,0,0);

	guint8* base = dest->get_pixels();
	guint8* alpha = src->get_pixels();
  
	for (int y=0; y<dest->get_height(); ++y)	
	{
		for (int x=0; x<dest->get_width(); ++x)
		{
			guint8* basePixel = base + y * dest->get_rowstride() + x * dest->get_n_channels();
			guint8* alphaPixel = alpha + y * src->get_rowstride() + x * src->get_n_channels();
			basePixel[3] = alphaPixel[0];
		}
	}
}
*/

void PixbufOps::CompositeMult(Glib::RefPtr<Gdk::Pixbuf> &dest, const Glib::RefPtr<Gdk::Pixbuf> &src, bool invertDest)
{
	assert(dest->get_height() == src->get_height());
	assert(dest->get_width() == src->get_width());
	assert(src->get_n_channels() >= 3);
	assert(dest->get_n_channels() >= 3);

	uint dest_rowstride = dest->get_rowstride();
	uint dest_n = dest->get_n_channels();
	guint8* dest_pixels = dest->get_pixels();

	uint src_rowstride = src->get_rowstride();
	uint src_n = src->get_n_channels();
	guint8* src_pixels = src->get_pixels();

	for (int y=0; y<dest->get_height(); ++y)	
	{
		for (int x=0; x<dest->get_width(); ++x)
		{
			guint8* destPixel = dest_pixels + y*dest_rowstride + x*dest_n;
			guint8* srcPixel = src_pixels + y*src_rowstride + x*src_n;
			float a = destPixel[0];
			float b = srcPixel[0] / 255.0;
			float v = a*b;
			destPixel[0] = destPixel[1] = destPixel[2] = (guint8)((invertDest)?255-v:v);
		}
	}
}

void PixbufOps::Superpose(Glib::RefPtr<Gdk::Pixbuf> &dest, const Glib::RefPtr<Gdk::Pixbuf> &src)
{
	assert(dest->get_height() == src->get_height());
	assert(dest->get_width() == src->get_width());
	assert(src->get_n_channels() >= 3);
	assert(dest->get_n_channels() >= 3);

	uint dest_rowstride = dest->get_rowstride();
	uint dest_n = dest->get_n_channels();
	guint8* dest_pixels = dest->get_pixels();

	uint src_rowstride = src->get_rowstride();
	uint src_n = src->get_n_channels();
	guint8* src_pixels = src->get_pixels();

	for (int y=0; y<dest->get_height(); ++y)	
	{
		for (int x=0; x<dest->get_width(); ++x)
		{
			guint8* destPixel = dest_pixels + y * dest_rowstride + x * dest_n;
			guint8* srcPixel = src_pixels + y * src_rowstride + x * src_n;
			for (int i=0; i<3; ++i)
			{
				int sum = destPixel[i] + srcPixel[i];
				if (sum > 255) sum = 255;
				destPixel[i] = sum;
			}
		}

	}
}

void PixbufOps::Multiply(Glib::RefPtr<Gdk::Pixbuf> &dest, float r, float g, float b)
{
	assert(dest->get_n_channels() >= 3);

	uint dest_rowstride = dest->get_rowstride();
	uint dest_n_channels = dest->get_n_channels();
	guint8* dest_pixels = dest->get_pixels();
	int h = dest->get_height();
	int w = dest->get_width();
	for (int y=0; y<h; ++y)	
	{
		for (int x=0; x<w; ++x)
		{
			guint8* basePixel = dest_pixels + y * dest_rowstride + x * dest_n_channels;
			basePixel[0] *= r;
			basePixel[1] *= g;
			basePixel[2] *= b;
		}
	}
}

void PixbufOps::Invert(Glib::RefPtr<Gdk::Pixbuf> &dest)
{
	assert(dest->get_n_channels() >= 3);
	uint dest_rowstride = dest->get_rowstride();
	uint dest_n_channels = dest->get_n_channels();
	guint8* dest_pixels = dest->get_pixels();

	for (int y=0; y<dest->get_height(); ++y)	
	{
		for (int x=0; x<dest->get_width(); ++x)
		{
			guint8* basePixel = dest_pixels + y * dest_rowstride + x * dest_n_channels;
			for (uint n=0; n<3; ++n)
				basePixel[n] = 255 - basePixel[n];
		}
	}
}

void PixbufOps::ShiftRange(Glib::RefPtr<Gdk::Pixbuf> &img, int min, int max)
{
	assert(img->get_n_channels() >= 3);
	uint rowstride = img->get_rowstride();
	uint n_channels = img->get_n_channels();
	guint8* pixels = img->get_pixels();

	for (int y=0; y<img->get_height(); ++y)	
	{
		for (int x=0; x<img->get_width(); ++x)
		{
			guint8* basePixel = pixels + y * rowstride + x * n_channels;
			for (uint n=0; n<3; ++n)
				basePixel[n] = basePixel[n]*(max-min)/255.0 + min;
		}
	}
}


void PixbufOps::SetPixel(Glib::RefPtr<Gdk::Pixbuf> &dest, int x, int y, float r, float g, float b)
{
	assert(dest->get_n_channels() >= 3);

	uint dest_rowstride = dest->get_rowstride();
	uint dest_n_channels = dest->get_n_channels();
	guint8* dest_pixels = dest->get_pixels();
	//int h = dest->get_height();
	//int w = dest->get_width();

	guint8* basePixel = dest_pixels + y * dest_rowstride + x * dest_n_channels;
	basePixel[0] = r;
	basePixel[1] = g;
	basePixel[2] = b;
}


void PixbufOps::CopyChannel(Glib::RefPtr<Gdk::Pixbuf> &img, int src, int dest)
{
	uint rowstride = img->get_rowstride();
	uint n_channels = img->get_n_channels();
	guint8* pixels = img->get_pixels();
	assert(src < (int)n_channels);
	assert(dest < (int)n_channels);
	for (int y=0; y<img->get_height(); ++y)	
	{
		for (int x=0; x<img->get_width(); ++x)
		{
			guint8* basePixel = pixels + y * rowstride + x * n_channels;
			basePixel[dest] = basePixel[src];
		}
	}
}

} // namespace gtk

} // namespace graphics
} // namespace latero


