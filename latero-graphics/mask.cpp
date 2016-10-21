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

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <Magick++.h>
#include <boost/filesystem/convenience.hpp>

#include "mask.h"
#include "gtk/pixbufops.h"


namespace latero {
namespace graphics { 

Mask::Mask(const Point &surfaceSize) :
	surfaceSize_(surfaceSize),
	centered_(true),
	offset_(0,0),
	size_(surfaceSize),
	srcTypeAlpha_(false),
	blur_(false),
	lockAspectRatio_(true),
	refMaximized_(true),
	wUnits_(units::mm),
	hUnits_(units::mm),
	defaultAlpha_(0)
{
	if (centered_) Center();
};

Mask::~Mask()
{
};

Glib::RefPtr<Gdk::Pixbuf> Mask::GetBlurredImage(std::string filename) const
{
	namespace fs = boost::filesystem;

	// created cache of blurred images if it doesn't already exist
	std::string cachePath = Glib::path_get_dirname(filename) + "/.blurred";
	fs::path boost_path(cachePath); //,fs::native);
	if (!fs::exists(boost_path))
	{
		fs::create_directories(boost_path);
		chmod(cachePath.c_str(), 0777); // make accessible to others than root
	}
	else if (!fs::is_directory(boost_path))
		printf("can't create cachePath!\n");

	// handle SVG images
	// must be converted to png before blurring
	if(filename.substr(filename.find_last_of(".")+1) == "svg")
	{
		std::string svgbitmap = cachePath + "/" + Glib::path_get_basename(filename) + ".png";

		fs::path boost_svg(filename); //,fs::native);
		fs::path boost_svgbitmap(svgbitmap); // ,fs::native);
		if (!fs::exists(boost_svgbitmap) || 
			(fs::last_write_time(boost_svgbitmap) < fs::last_write_time(boost_svg)))
		{
			Glib::RefPtr<Gdk::Pixbuf> svgbuf = Gdk::Pixbuf::create_from_file(filename);
			svgbuf->save(svgbitmap,"png");
			chmod(svgbitmap.c_str(), 0666); // make accessible to others than root
		}
		filename = svgbitmap;
	}

	// create blurred file if it doesn't exist or is too old
	std::string blurredFile = cachePath + "/" + "blurred_" + Glib::path_get_basename(filename);
	fs::path boost_blurred(blurredFile); //,fs::native);
	fs::path boost_normal(filename); // ,fs::native);
	if (!fs::exists(boost_blurred) || 
		(fs::last_write_time(boost_blurred) < fs::last_write_time(boost_normal)))
	{
		printf("Updating blurred cache: %s...\n", blurredFile.c_str());
 		if (srcTypeAlpha_)
		{
			// can't use Magick++ until version with blurChannel is available
			std::stringstream cmd;
			cmd << "convert " << filename << " -channel A -blur 4x2 "<< blurredFile; 
			system(cmd.str().c_str());
		}
		else
		{
			Magick::Image image; 
			image.read(filename);
			image.blur(4,2); // gaussian blur, radius of 4, sigma of 2
			image.write(blurredFile.c_str());
		}
		chmod(blurredFile.c_str(), 0666); // make accessible to others than root

	}

	return Gdk::Pixbuf::create_from_file(blurredFile);
}

bool Mask::SetImage(Glib::RefPtr<Gdk::Pixbuf> img, bool alpha)
{
	srcTypeAlpha_ = alpha;

	// TODO ignoring blur right now!
	Glib::RefPtr<Gdk::Pixbuf> cp = img->copy();

	// we're doing the strict mininum inside this lock...
	LATERO_GRAPHICS_GUARD;
	srcFile_ = "";
	float hscale = GetWidth(units::percent);
	float vscale = GetHeight(units::percent);
	img_=cp;
	if (!alpha)
	{
		img_=img_->add_alpha(false,0,0,0);
		gtk::PixbufOps::CopyChannel(img_,0,3);
	}
	SetSize(hscale, vscale, units::percent);
	UpdateShadow();
	return true;
}

bool Mask::SetImage(std::string filename, bool alpha)
{
	namespace fs = boost::filesystem;

	// check if the file exists
	fs::path boost_filename(filename); //,fs::native);
	if (!fs::exists(boost_filename))
	{
		std::cout << "Mask::SetImage(" << filename << "): file doesn't exist" << std::endl;
		return false;
	}

	// load the image
	srcTypeAlpha_ = alpha;
	try
	{
		Glib::RefPtr<Gdk::Pixbuf> buf =
			IsBlurred()?GetBlurredImage(filename):Gdk::Pixbuf::create_from_file(filename);

		// we're doing the strict mininum inside this lock...
		LATERO_GRAPHICS_GUARD;
		srcFile_ = filename;
		float hscale = GetWidth(units::percent);
		float vscale = GetHeight(units::percent);
		img_=buf;
		if (!alpha)
		{
			img_=img_->add_alpha(false,0,0,0);
			gtk::PixbufOps::CopyChannel(img_,0,3);
		}
		SetSize(hscale, vscale, units::percent);
		UpdateShadow();
		return true;
	}
	catch (Glib::FileError &ex)
	{
		std::cout << "Mask::SetImage(" << filename << "): could not load image" << std::endl;
		return false;
	}
}

void Mask::LoadXML(const XMLInputNode &root)
{
	XMLInputNode maskNode = root.GetChild("mask");

	XMLInputNode node = maskNode.GetChild("imgfile");
	printf("type: %s\n", node.GetType().c_str());
	SetImage(node.GetFilename(), (node.GetType()=="alpha"));

	// temporary
	//node = maskNode.GetChild("scale");
	//double scale = (node)?node.GetFloat():1;
	//SetSize(scale, scale, units::percent);

	// TODO: reloads image if blur changes!
	node = maskNode.GetChild("blur");
	if (node)
		SetBlurred(node.GetBool());
	else
		SetBlurred(false);

	node = maskNode.GetChild("position");
	if (node)
	{
		if (node.GetString() == "centered")
			SetCentered(true);
		else
			SetOffset(node.GetPoint());
	}

	node = maskNode.GetChild("lock_aspect_ratio");
	if (node) SetLockAspectRatio(node.GetBool());
	
	node = maskNode.GetChild("reference_maximized");
	if (node) SetRefMaximized(node.GetBool());

	bool ar = GetLockAspectRatio();
	SetLockAspectRatio(false);

	node = maskNode.GetChild("width");
	if (node) SetWidth(node.GetFloat(), node.GetUnits());

	node = maskNode.GetChild("height");
	if (node) SetHeight(node.GetFloat(), node.GetUnits());

	SetLockAspectRatio(ar);

	node = maskNode.GetChild("default_alpha");
	if (node) SetDefaultAlpha(node.GetFloat());
}

void Mask::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode maskNode = root.AddChild("mask");

	XMLOutputNode imgfileNode = maskNode.AddFilenameChild("imgfile", GetImgFile());
	imgfileNode.SetAttribute("type",(srcTypeAlpha_?"alpha":"gray"));

	if (IsBlurred()) maskNode.AddChild("blur", IsBlurred());
	if (GetCentered())
		maskNode.AddChild("position", "centered");
	else
		maskNode.AddChild("position", GetOffset(), units::mm);
	maskNode.AddChild("lock_aspect_ratio", GetLockAspectRatio());
	maskNode.AddChild("reference_maximized", GetRefMaximized());
	maskNode.AddChild("width", GetWidth(wUnits_), wUnits_);
	maskNode.AddChild("height", GetHeight(hUnits_), hUnits_);
	maskNode.AddChild("default_alpha", GetDefaultAlpha(), units::percent);
}


Glib::RefPtr<Gdk::Pixbuf> Mask::GetIllustration(uint w, uint h)
{
	//
	// Destination
	//
	Glib::RefPtr<Gdk::Pixbuf> rv = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, w, h);
	rv->fill(0xffffffff);
	Cairo::RefPtr<Cairo::ImageSurface> rvSurface = Cairo::ImageSurface::create(
    		(unsigned char*)rv->get_pixels(),
    		Cairo::FORMAT_ARGB32, rv->get_width(), rv->get_height(), rv->get_rowstride());
	Cairo::RefPtr<Cairo::Context> rvContext = Cairo::Context::create(rvSurface);

	//
	// Source
	//
	Glib::RefPtr<Gdk::Pixbuf> alpha = GetSurfaceAlpha(w,h);
	if (!alpha) return rv;
	Cairo::RefPtr<Cairo::ImageSurface> alphaSurface = Cairo::ImageSurface::create(
    		(unsigned char*)alpha->get_pixels(),
    		Cairo::FORMAT_ARGB32, alpha->get_width(), alpha->get_height(), alpha->get_rowstride());


	rvContext->set_source_rgb(0,0,0);
	rvContext->mask(alphaSurface,0,0);

	Point offset = GetOffset();
	Point size = GetSize();

	rvContext->set_source_rgb(0,0,1);
	rvContext->scale(w/surfaceSize_.x, h/surfaceSize_.y);
	rvContext->translate(offset.x,offset.y);
	rvContext->move_to(0,0);
	rvContext->line_to(size.x,0);
	rvContext->line_to(size.x,size.y);
	rvContext->line_to(0,size.y);
	rvContext->close_path();
	rvContext->stroke();
	
	return rv;
}

double Mask::Get_(const Point &pos) const
{
	if (!img_) return defaultAlpha_;

	// find normalized position (0-1) within mask window
	Point winPos;
	winPos.x = (pos.x - offset_.x) / size_.x;
	winPos.y = (pos.y - offset_.y) / size_.y;
	
	// check if out of bounds
	if ((winPos.x<0)||(winPos.x>=1)||(winPos.y<0)||(winPos.y>=1))
		return defaultAlpha_;

	// find the value of the corresponding pixel
	int i = (int)floor(winPos.x * img_->get_width());
	int j = (int)floor(winPos.y * img_->get_height());
	guint8* pixel = img_->get_pixels() + j*img_->get_rowstride() + i*img_->get_n_channels();

	return pixel[3] / 255.0;
}

double Mask::GetShadow_(const Point &pos) const
{
	if (!shadow_) return defaultAlpha_;

	// find normalized position (0-1) within mask window
	Point winPos;
	winPos.x = (pos.x - offset_.x) / size_.x;
	winPos.y = (pos.y - offset_.y) / size_.y;
	
	// check if out of bounds
	if ((winPos.x<0)||(winPos.x>=1)||(winPos.y<0)||(winPos.y>=1))
		return defaultAlpha_;

	// find the value of the corresponding pixel
	int i = (int)floor(winPos.x * img_->get_width());
	int j = (int)floor(winPos.y * img_->get_height());
	guint8* pixel = shadow_->get_pixels() + j*shadow_->get_rowstride() + i*shadow_->get_n_channels();

	return pixel[3] / 255.0;
}


bool Mask::IsBlurred() const
{
	LATERO_GRAPHICS_GUARD;
	return blur_;
}

void Mask::SetBlurred(bool v)
{
	LATERO_GRAPHICS_LOCK;
	bool changed = (blur_ != v);
	blur_ = v;
	LATERO_GRAPHICS_UNLOCK;

	// reload image if necessary
	if (changed && (srcFile_!=""))
		SetImage(srcFile_,srcTypeAlpha_);
}

void Mask::SetSize(float width, float height, std::string units)
{
	bool ar = GetLockAspectRatio();
	SetLockAspectRatio(false);
	SetWidth(width,units);
	SetHeight(height,units);
	SetLockAspectRatio(ar);
}

void Mask::SetWidth(float v, std::string units)
{
	LATERO_GRAPHICS_GUARD;
	if (units==units::percent)
	{
		if (refMaximized_)
			v = v * GetMaximizedSize_().x;
		else
			v = v * surfaceSize_.x;
	}
	if (lockAspectRatio_)
		size_.y *= v / size_.x;
	size_.x =  v;
	if (centered_) Center();
	SetLastModified_();
	wUnits_ = units;
}

double Mask::GetWidth(std::string units) const 
{
	LATERO_GRAPHICS_GUARD;
	if (units==units::percent)
	{
		if (refMaximized_)
			return size_.x / GetMaximizedSize_().x;
		else
			return size_.x / surfaceSize_.x;
	}
	else
		return size_.x;
}

void Mask::SetHeight(float v, std::string units)
{
	LATERO_GRAPHICS_GUARD;
	if (units==units::percent)
	{
		if (refMaximized_)
			v = v * GetMaximizedSize_().y;
		else
			v = v * surfaceSize_.y;
	}
	if (lockAspectRatio_)
		size_.y *= v / size_.y;
	size_.y =  v;
	if (centered_) Center();
	SetLastModified_();
	hUnits_ = units;
}

double Mask::GetHeight(std::string units) const 
{
	LATERO_GRAPHICS_GUARD;
	if (units==units::percent)
	{
		if (refMaximized_)
			return size_.y / GetMaximizedSize_().y;
		else
			return size_.y / surfaceSize_.y;
	}
	else
		return size_.y;
}

void Mask::Center()
{
	LATERO_GRAPHICS_GUARD;
	offset_ = (surfaceSize_-size_)/2.0;
	SetLastModified_();
}

Point Mask::GetMaximizedSize_() const
{
	if (!img_) return surfaceSize_;

	float surfaceRatio = surfaceSize_.x/surfaceSize_.y;
	float targetRatio = (float)img_->get_width()/(float)img_->get_height();

	Point winSize = surfaceSize_;
	if (surfaceRatio > targetRatio)
		winSize.x = winSize.y * targetRatio;
	else
		winSize.y = winSize.x / targetRatio;
	return winSize;
}


Glib::RefPtr<Gdk::Pixbuf> Mask::GetImgCopy() const
{
	Glib::RefPtr<Gdk::Pixbuf> rv = GetAlphaImgCopy();
	gtk::PixbufOps::CopyChannel(rv,3,0);
	gtk::PixbufOps::CopyChannel(rv,3,1);
	gtk::PixbufOps::CopyChannel(rv,3,2);
	return rv;
}

Glib::RefPtr<Gdk::Pixbuf> Mask::GetAlphaImgCopy() const
{
	LATERO_GRAPHICS_GUARD;
	Glib::RefPtr<Gdk::Pixbuf> rv;
	if (img_) rv = img_->copy();
	return rv;
}


Glib::RefPtr<Gdk::Pixbuf> Mask::GetSurfaceAlpha(uint w, uint h)
{
	Glib::RefPtr<Gdk::Pixbuf> rv = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, w, h);
	rv->fill(defaultAlpha_*0x000000ff);

	Glib::RefPtr<Gdk::Pixbuf> src = GetAlphaImgCopy();
	if (!src) return rv;

	Point size = GetSize();
	int size_x = size.x * w / surfaceSize_.x;
	int size_y = size.y * h / surfaceSize_.y;

	Point offset = GetOffset();
	int offset_x = offset.x * w / surfaceSize_.x;
	int offset_y = offset.y * h / surfaceSize_.y;

	if ((size_x>=1)&&(size_y>= 1))
	{
		src = src->scale_simple(size_x, size_y, Gdk::INTERP_BILINEAR);

		int src_x1 = fmax(0,-offset_x);
		int src_y1 = fmax(0,-offset_y);
		int src_x2 = fmin(size_x, w - offset_x);
		int src_y2 = fmin(size_y, h - offset_y);
		int src_w = src_x2-src_x1;
		int src_h = src_y2-src_y1;

		int dest_x = fmax(offset_x,0);
		int dest_y = fmax(offset_y,0);

		if ((dest_x>=0)&&(dest_y>=0)&&(src_w>=0)&&(src_h>=0))
			src->copy_area(src_x1, src_y1, src_w, src_h, rv, dest_x, dest_y);
	}
    
    return rv;
}

void Mask::ReloadImage()
{
	if (srcFile_!="")
		SetImage(srcFile_, srcTypeAlpha_);
}

void Mask::UpdateShadow()
{
	// TODO: cache this!!!!

	if (img_)
	{
		shadow_ = img_->copy();

		uint rowstride = shadow_->get_rowstride();
		uint n = shadow_->get_n_channels();
		guint8* pixels = shadow_->get_pixels();
		int w = shadow_->get_width();
		int h = shadow_->get_height();

		// saturate
		for (int y=0; y<h; ++y)	
		{
			for (int x=0; x<w; ++x)
			{
				guint8* pixel = pixels + y * rowstride + x * n;
				if (pixel[3]>0) pixel[3]=255; // saturate
			}
		}

		// dilate with a gradient...
		Glib::RefPtr<Gdk::Pixbuf> dilated = shadow_->copy();
		int size = shadow_size;
		for (int y=0; y<h; ++y)	
		for (int x=0; x<w; ++x)
		{
			guint8* pixel = pixels + y * rowstride + x * n;
			if (pixel[3]>0)
			{
				for (int xx=x-size; xx<x+size; ++xx)
				for (int yy=y-size; yy<y+size; ++yy)
				{
					if ((xx>=0)&&(xx<w)&&(yy>=0)&&(yy<h))
					{
						guint8* p = dilated->get_pixels() + yy * rowstride + xx * n;
						if (p[3] < 255)
						{
							double d = sqrt((xx-x)*(xx-x) + (yy-y)*(yy-y)) / size;
							int value = 255 * fmax(0, 1 - d);
							if (value > p[3])
								p[3] = value;
						}
					}
				}
			}
		}
		shadow_ = dilated;
	}
}

bool Mask::IsInside(const Point &p)
{
	return 	(p.x >= offset_.x) &&
		(p.y >= offset_.y) &&
		(p.x <= offset_.x + size_.x) &&
		(p.y <= offset_.y + size_.y);
}

} // namespace graphics
} // namespace latero
