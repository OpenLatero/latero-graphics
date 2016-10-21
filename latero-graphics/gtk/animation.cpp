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

#include "animation.h"
#include "pixbufops.h"
#include <sstream>
#include <iostream>
#include <assert.h>
#include <boost/filesystem/convenience.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include "../external.h"

namespace latero {
namespace graphics { 

namespace gtk {

Animation::~Animation()
{
	Deactivate();
}

Animation::Animation() :
	active_(false)
{
}

Animation::Animation(Glib::RefPtr<Gdk::Pixbuf> &frame)
{
	imgs_.push_back(frame);
}

Animation::Animation(const Animation& p)
{
	imgs_ = p.imgs_;
};

Animation Animation::operator= (const Animation& p)
{ 
	if (&p == this) return *this;
	imgs_ = p.imgs_;
	return *this;
};

Animation Animation::Clone() const
{
	Animation rv;
	for (uint i=0; i<imgs_.size(); ++i)
		rv.AppendFrame(imgs_[i]->copy());
	return rv;
}

void Animation::AppendFrame(Glib::RefPtr<Gdk::Pixbuf> frame)
{
	if (GetNbFrames() && (((int)frame->get_width() != GetWidth())||((int)frame->get_height() != GetHeight())))
	{
		printf("Animation::AppendFrame invalid frame size: got (%d,%d), expected (%d,%d)\n", 
			frame->get_width(), frame->get_height(), GetWidth(), GetHeight());
		return;
	}
	imgs_.push_back(frame);
}

void Animation::Clear()
{
	imgs_.clear();
}

void Animation::Multiply(float r, float g, float b)
{
	for (uint i=0; i<GetNbFrames(); ++i)
		PixbufOps::Multiply(imgs_[i], r, g, b);
}

Glib::RefPtr<Gdk::Pixbuf> Animation::GetCurrentFrame()
{
	Glib::RefPtr<Gdk::Pixbuf> rv;
	if (GetNbFrames())
		rv = imgs_.at(tick_%GetNbFrames());
	return rv;
}

void Animation::Deactivate()
{
	if (IsActive())
	{
		active_ = false;
		timeout_.disconnect();
	}
}

void Animation::Activate(uint interval)
{
	if (IsActive())
		Deactivate();

	// We could activate this signal only if there's more than one frame. The downside is that
	// we'd still have to note the activation request and enable the signal later if more frames are
	// added. It's simpler and less error prone to just activate the signal no matter what. The performance 
	// cost should be minimal.

	active_ = true;
	timeout_ = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &Animation::OnTimer),
		interval,
		Glib::PRIORITY_DEFAULT_IDLE);
}

bool Animation::OnTimer()
{
	IncrementFrame();
	return true;
}

void Animation::IncrementFrame()
{
	tick_ = (tick_+1)%GetNbFrames();
	if (GetNbFrames() > 1)
		signal_current_frame_changed();
}

int Animation::GetCurrentFrameIndex()
{
	return tick_;
}

void Animation::DecrementFrame()
{
	tick_ = (tick_>0) ? tick_-1 : GetNbFrames() - 1;
	if (GetNbFrames() > 1)
		signal_current_frame_changed();
}


void Animation::CompositeMult(const Animation &src)
{
	if (!GetNbFrames()) return ;

	assert(GetWidth() == src.GetWidth());
	assert(GetHeight() == src.GetHeight());

	// add frames if we're smaller than src	
	while (GetNbFrames() < src.GetNbFrames())
		imgs_.push_back(imgs_[GetNbFrames()-1]->copy());

	for (unsigned int f=0; f<GetNbFrames(); ++f)
	{
		Glib::RefPtr<Gdk::Pixbuf> frame = src.imgs_[f%src.GetNbFrames()]; // stay stuck on last frame if dest if bigger
		PixbufOps::CompositeMult(imgs_[f], frame, false);
	}	
}

void Animation::SaveToFile(std::string file, bool trim)
{
	namespace fs = boost::filesystem;	

	std::string ext = fs::extension(file);
	std::string base = fs::basename(file);
	
	try
	{
		if (GetNbFrames() == 1)
		{
			std::string filename = base+".png";
			imgs_[0]->save(filename,"png");
			chmod(filename.c_str(), 0666); // make accessible to others than root
		}
		else
		{
			// save individual frames
			std::vector<std::string> frames;
			for (uint i=0; i<GetNbFrames(); ++i)
			{
				char pngfile[255];
				sprintf(pngfile, "%s-%05d.png", base.c_str(), i);
				frames.push_back(pngfile);
				imgs_[i]->save(pngfile,"png");
				chmod(pngfile, 0666); // make accessible to others than root
			}

			// save gif animation
			std::string output = base + ".gif";
			SysCommand convertCmd("convert");
			convertCmd.Append("-delay 5 -loop 0");
			convertCmd.Append(frames);
			convertCmd.Append(output);
			convertCmd.Run();
			chmod(output.c_str(), 0666); // make accessible to others than root

			// save swf animation
			output = base + ".swf";
			SysCommand png2swf("png2swf");
			png2swf.Append("-o");
			png2swf.Append(output);
			png2swf.Append("-r 30");
			png2swf.Append(frames);
			png2swf.Run();
			chmod(output.c_str(), 0666); // make accessible to others than root


			// zip the png sources together
			output = base + "-png.tar";
			SysCommand tar("tar");
			tar.Append("-cvf");
			tar.Append(output);
			tar.Append(frames);
			tar.Run();
			chmod(output.c_str(), 0666); // make accessible to others than root

			// keep the first png
			output = base+".png";
			SysCommand cp("cp");
			cp.Append(frames[0]);
			cp.Append(output);
			cp.Run();
			chmod(output.c_str(), 0666); // make accessible to others than root

			// remove png sources
			SysCommand rm("rm");
			rm.Append("-rf");
			rm.Append(frames);
			rm.Run();
		}
	}
	catch (Gdk::PixbufError& ex)
	{
		std::cout << ex.what() << "\n";
	}
}

void Animation::SaveToFile()
{
	Gtk::FileChooserDialog dialog("Please select file name.", Gtk::FILE_CHOOSER_ACTION_SAVE);
	std::string dir = Glib::get_current_dir();
	dialog.set_current_folder(dir);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	dialog.set_default_response(Gtk::RESPONSE_CANCEL);
	dialog.set_current_name("animation.png");

	if (Gtk::RESPONSE_OK == dialog.run())
	{
		printf("saving animation to %s\n", dialog.get_filename().c_str());
		SaveToFile(dialog.get_filename());
	}
}




void Animation::CompositeMult(const std::vector<latero::graphics::gtk::Animation> &anims)
{
	if (anims.size())
	{
		*this = anims[0];
		for (uint i=1; i<anims.size(); ++i)
			CompositeMult(anims[i]);
	}
}

Animation Animation::ScaleSimple(int dest_width, int dest_height, Gdk::InterpType interp_type) const
{
	Animation rv;
	for (unsigned int f=0; f<GetNbFrames(); ++f)
		rv.AppendFrame(imgs_[f]->scale_simple(dest_width, dest_height, interp_type));
	return rv;
}

bool Animation::IsActive()
{
	return active_;
}

} // namespace gtk

} // namespace graphics
} // namespace latero

