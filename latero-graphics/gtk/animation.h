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

#ifndef GTKVL_ANIMATION
#define GTKVL_ANIMATION

#include <gdkmm/pixbuf.h>

namespace latero {
namespace graphics { 

namespace gtk {

/**
 * This class implements an animation as a container of frames. It also supports automatic update of
 * the currently displayed frame, along with notification using signals. Unless otherwise noted, all
 * operations use reference counted pointers to the frames it is given and doesn't create its own copies.
 *
 * @todo The class Glib::PixbufAnimation is much better than this but it can only be used by loading images from
 *       an animated GIF. The Glib Animation class that support modification by users is not wrappers in Gtkmm yet.
 * @todo This could possibly be turned into a widget much like Gtk::Image.
 */
class Animation
{
public:

	/********************************************************
	 * Constructor / Destructor
	 ********************************************************/

	/** creates an empty animation */
	Animation();

	/** creates an animation with a single frame */
	Animation(Glib::RefPtr<Gdk::Pixbuf> &frame);

	/** copy constructor */
	Animation(const Animation& p);

	/** copy operator */
	Animation operator= (const Animation& p);

	virtual ~Animation();

	/**
	 * Creates a copy of the animation, including all of its frames. The pixbufs of the
	 * returned Animation are independent copies of the originals.
	 */
	Animation Clone() const;

	/** scale each frame of the animation (see Gtk::Pixbuf::scale_simple) */
	Animation ScaleSimple(int dest_width, int dest_height, Gdk::InterpType interp_type) const;

	/** allows animation to be interpreted as a single frame */
 	operator Glib::RefPtr<Gdk::Pixbuf>() { return GetCurrentFrame(); }

	/********************************************************
	 * Frames & operations
	 ********************************************************/

	/** @return currently shown frame (see Activate()) */
	Glib::RefPtr<Gdk::Pixbuf> GetCurrentFrame();

	/** append a frame at the end */
	void AppendFrame(Glib::RefPtr<Gdk::Pixbuf> frame);
	
	/** empty animation */
	void Clear();

	/**
	 * @param file Complete path of the filename. If more than one frame is used, a number will be inserted.
	 * saves each frame of the animation to a file 
	 * @param trim auto-crops the image if true
	 */
	void SaveToFile(std::string file, bool trim = false);

	/**
	 * Interaction version of SaveToFile. Popups a dialog to select the filename.
	 */
	void SaveToFile();

	/** @return frame width or 0 if empty */
	inline int GetWidth() const { return GetNbFrames()?imgs_[0]->get_width():0; }

	/** @return frame height or 0 if empty */
	inline int GetHeight() const { return GetNbFrames()?imgs_[0]->get_height():0; }

	/** @return number of frames */
	inline uint GetNbFrames() const { return imgs_.size(); }

	/** manually skip to the next frame */
	void IncrementFrame();

	/** manually skip to the previous frame */
	void DecrementFrame();

	/** return the number of the current frame */
	int GetCurrentFrameIndex();

	/********************************************************
	 * Automatic refresh
	 ********************************************************/

	/** 
	 * @param interval frame interval in ms
 	 * activate automatic frame incrementation 
	 */
	void Activate(uint interval);

	/** deactivate automatic frame incrementation */
	void Deactivate();

	/** returns whether the animation has been activated */
	bool IsActive();

	/** signal raised when current frame changes (if activated) */
	sigc::signal<void> signal_current_frame_changed;

	/********************************************************
	 * Image processing operations
	 ********************************************************/

	/** Multiply each pixel by these r,g,b ratios. */
	void Multiply(float r, float g, float b);

	/**
	 * Multiplies each frame of the current animation by those of the source animation, pixel by pixel. If
	 * an animation is smaller than the other, its last frame is repeated until they match in length.
	 */
	void CompositeMult(const Animation &src);
	void CompositeMult(const std::vector<latero::graphics::gtk::Animation> &anims);

protected:

	bool OnTimer();

	bool active_;
	std::vector< Glib::RefPtr<Gdk::Pixbuf> > imgs_;
	uint tick_;
	sigc::connection timeout_;
};

} // namespace gtk

} // namespace graphics
} // namespace latero

#endif
