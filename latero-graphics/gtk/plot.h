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

#ifndef GTKVL_PLOT_H
#define GTKVL_PLOT_H

#include <queue>
#include <gtkmm.h>

namespace latero {
namespace graphics { 

namespace gtk {

class Plot : public Gtk::DrawingArea
{
public:
	/**
	 * @param fgColor curve color
	 * @param bgColor background color
	 */
	Plot(const char *fgColor = "blue", const char *bgColor = "white");
	virtual ~Plot();


	/** clear the display */
	void Clear();

	/** 
	 * Insert a new point to be drawn on next call to Draw().
	 * @param x		in um
	 * @param y		-1.0 to 1.0
	 */
	void InsertPoint(float x, float y);

	/** Draw all points accumulated by calls to InsertPoint(...) */
	void Draw();

	void AddChannel(const char *fgColor);
	void ClearChannel(unsigned int channel);
	void InsertPoint(unsigned int channel, float x, float y);
	unsigned int GetNbChannels() { return channels_.size(); }

	void SetRangeX(float min, float max);
	void SetRangeY(float min, float max);

	float GetMinX() { return xMin_; }
	float GetMaxX() { return xMax_; }
	float GetMinY() { return yMin_; }
	float GetMaxY() { return yMax_; }

	void Draw(Cairo::RefPtr<Cairo::Context> cr, uint w, uint h, bool gtkmode = false);
	void SaveToFile(std::string filename, uint w, uint h);
	void OnSaveAs();
	void OnSave();

protected:
	void CreatePopupMenu();
	bool OnClick(GdkEventButton* event);
	virtual bool on_expose_event(GdkEventExpose* event);

	const Gdk::Color bgColor_;	// background
	const Gdk::Color lineColor_;	// line in the middle of the screen

	typedef struct struct_point
	{
		float x;
		float y;
	} Point;

	typedef struct
	{
		std::vector<Point> points;
		Gdk::Color color;
	} Trace;

	Glib::RefPtr<Gtk::UIManager> uiManager_;
	std::vector<Trace> channels_;

	float xMin_, xMax_;
	float yMin_, yMax_;
};

} // namespace gtk

} // namespace graphics
} // namespace latero

#endif
