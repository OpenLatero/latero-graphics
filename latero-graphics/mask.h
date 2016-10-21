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

#ifndef LATERO_GRAPHICS_MASK_H
#define LATERO_GRAPHICS_MASK_H

#include <gdkmm/pixbuf.h>

#include "maskfwd.h"
#include "modifiable.h"
#include "units.h"
#include "xml.h"

namespace latero {
namespace graphics { 

/**
 * A Mask maps a bitmapped image to a window within the virtual surface.
 */
class Mask : public Modifiable
{
public:
	/**
	 * Creator
	 * @param surfaceSize size of virtual surface (mm)
	 */
	static MaskPtr Create(const Point &surfaceSize)
	{
		return MaskPtr(new Mask(surfaceSize));
	}

	/**
	 * destructor 
	 */
	virtual ~Mask();

	/**
	 * load parameters from an XML node
	 * @param node XML node
	 * @param path base path for relative filenames included in XML
	 */
	void LoadXML(const XMLInputNode &node);

	/**
	 * adds parameters to an XML node
	 * @param root XML node
	 */
	void AppendXML(XMLOutputNode &root) const;

	/**
	 * @param pos position within virtual surface (mm)
	 * @return mask value (0-1) at that position
	 */
	inline float Get(const Point &pos)
	{
		LATERO_GRAPHICS_GUARD;
		return Get_(pos);
	}

	/**
	 * @param pos position within virtual surface (mm)
	 * @return mask value (0-1) at that position
	 * @warning non-locking
	 */
	double Get_(const Point &pos) const;

	double GetShadow_(const Point &pos) const;

	/**
	 * @param filename image file to load as a new mask
	 * @param alpha alpha channel is used if true
	 * @return true on success
	 */
	bool SetImage(std::string filename, bool alpha=false);

	/**
	 * @param img buffer to copy
	 * @param alpha alpha channel is used if true
	 * @return true on success
	 * @warning NOT WELL IMPLEMENTED... review
	 */
	bool SetImage(Glib::RefPtr<Gdk::Pixbuf> img, bool alpha=false);

	void ReloadImage();

	/** 
	 * @return illustration corresponding to the mask over the entire virtual surface
	 */
	Glib::RefPtr<Gdk::Pixbuf> GetIllustration(uint w, uint h);

	/** @return size of the window in mm */
	inline Point GetSize() const { LATERO_GRAPHICS_GUARD; return size_; }
	inline Point GetSize_() const { return size_; }

	/** @return offset of the window in mm  */
	inline Point GetOffset() const { LATERO_GRAPHICS_GUARD; return offset_; }
	inline Point GetOffset_() const { return offset_; }

	/** set offset of the window in mm  (overrides centered_) */
	inline void SetOffset(const Point& p) { LATERO_GRAPHICS_GUARD; offset_=p; SetCentered(false); }

	/**
	 * Set the width of the window to a specific size.
	 * @param width width of window in mm or relative to the maximized size
	 * @param units units::mm or units::percent
	 */
	void SetWidth(float width, std::string units = units::mm);
	double GetWidth(std::string units = units::mm) const ;
	// TODO: which units do we save with?

	/**
	 * Set the height of the window to a specific size.
	 * @param width height of window in mm or relative to the maximized size
	 * @param units units::mm or units::percent
	 */
	void SetHeight(float height, std::string units = units::mm);
	double GetHeight(std::string units = units::mm) const ;

	/** overrides aspect ratio lock */
	void SetSize(float width, float height, std::string units = units::mm);

	/** @return filename of image file */
	inline std::string GetImgFile() const
	{
		LATERO_GRAPHICS_GUARD;
		return srcFile_;
	}

	/** @return true if mask image is blurred */
	bool IsBlurred() const;

	/** Set whether the mask image should be blurred. */
	void SetBlurred(bool v = true);

	/** Identical to GetIllustration() but alpha channel is set to copy grayscale level. */
	Glib::RefPtr<Gdk::Pixbuf> GetSurfaceAlpha(uint w, uint h);

	/** Set the position of the window such that its center is in the center of the virtual surface. */
	void Center();

	/** Select whether the window should remain centered at all times */ 
	void SetCentered(bool v) { LATERO_GRAPHICS_GUARD; centered_=v; SetLastModified_(); if (v) Center(); }

	/** @return whether the window should remain centered at all times */ 
	bool GetCentered() const { LATERO_GRAPHICS_GUARD; return centered_; }

	/** @return size of the virtual surface */
	const Point &GetSurfaceSize() const  { return surfaceSize_; }

	void SetLockAspectRatio(bool v) { lockAspectRatio_=v; }
	bool GetLockAspectRatio() const { return lockAspectRatio_; }
	
	void SetRefMaximized(bool v) { refMaximized_=v; }
	bool GetRefMaximized() const { return refMaximized_; }

	std::string GetWidthUnits() const { return wUnits_; }
	std::string GetHeightUnits() const { return hUnits_; }

	void SetWidthUnits(std::string u) { wUnits_ = u; }
	void SetHeightUnits(std::string u) { hUnits_ = u; }

	void SetDefaultAlpha(double v) { LATERO_GRAPHICS_GUARD; defaultAlpha_=v; SetLastModified_(); }
	double GetDefaultAlpha() const { LATERO_GRAPHICS_GUARD; return defaultAlpha_; }
	double GetDefaultAlpha_() const { return defaultAlpha_; }

	void SetSrcTypeAlpha(bool v) { srcTypeAlpha_=v; ReloadImage(); }
	bool GetSrcTypeAlpha() const { return srcTypeAlpha_; }


	/** @returns a copy of the image buffer (if it exists) - as grayscale, not alpha (deprecated) */
	Glib::RefPtr<Gdk::Pixbuf> GetImgCopy() const;

	bool IsInside(const Point &p);

protected:

	void UpdateShadow();

	/** @returns a copy of the image buffer (if it exists) */
	Glib::RefPtr<Gdk::Pixbuf> GetAlphaImgCopy() const;

	/**
	 * This method computes the size of the current image's window when maximized such that the image aspect
	 * ratio is respected and the image occupies as much space as possible in the virtual surface. This is the
	 * reference dimension used when applying a scaling factor to the mask.
	 */
	Point GetMaximizedSize_() const;

	/**
	 * This method returns a blurred version of the image specified by filename. Blurred images are cached in a subdirectory
	 * named ".blurred" at the location where the source image resides. The blurred image is created and cached if it
	 * doesn't exist already, or is updated if it exists but is older than the source image.
	 *
	 * @param filename path of the source image file before blurring
	 * @return image buffer of blurred image
	 */
	Glib::RefPtr<Gdk::Pixbuf> GetBlurredImage(std::string filename) const;

protected:

	/**
	 * constructor
	 * @param surfaceSize size of virtual surface (mm)
	 */
	Mask(const Point &surfaceSize);

	const Point surfaceSize_; 	/** size of the virtual surface (mm) */
	bool centered_;			/** determines whether the window is always centered */
	Point offset_;			/** position of upper-left corner of window within the virtual surface (mm) */
	Point size_;			/** size of window that the mask occupies (mm) */
	Glib::RefPtr<Gdk::Pixbuf> img_;	/** image buffer (alpha is all that counts) */
	Glib::RefPtr<Gdk::Pixbuf> shadow_; /** image buffer (alpha is all that counts) */
	std::string srcFile_;		/** source file for image buffer (if available) */
	bool srcTypeAlpha_;		/** if true, the alpha channel of the source is used */
	bool blur_;			/** image is blurred if true */
	bool lockAspectRatio_;		/** aspect ratio is maintained when locked */
	bool refMaximized_;		/** use maximized as reference instead of surface for relative measures */
	std::string wUnits_, hUnits_;	/** last used units for width and height */
	double defaultAlpha_;		/** alpha value when there is no mask (0-1) */

	static const int shadow_size = 5; // TODO: could go outside buffer...
};

} // namespace graphics
} // namespace latero

#endif
