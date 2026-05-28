#pragma once

#include <queue>
#include <gtkmm.h>

namespace latero::graphics::gtk {

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
	unsigned int GetNbChannels() { return (unsigned int)channels_.size(); }

	void SetRangeX(float min, float max);
	void SetRangeY(float min, float max);

	float GetMinX() { return xMin_; }
	float GetMaxX() { return xMax_; }
	float GetMinY() { return yMin_; }
	float GetMaxY() { return yMax_; }

	void Draw(Cairo::RefPtr<Cairo::Context> cr, int w, int h, bool gtkmode = false);
	void SaveToFile(std::string filename, uint w, uint h);
	void OnSaveAs();
	void OnSave();

protected:
	void CreatePopupMenu();
	void OnClick(int n_press, double x, double y);
	void OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h);

	const Gdk::RGBA bgColor_;	// background
	const Gdk::RGBA lineColor_;	// line in the middle of the screen

	typedef struct struct_point
	{
		float x;
		float y;
	} Point;

	typedef struct
	{
		std::vector<Point> points;
		Gdk::RGBA color;
	} Trace;

	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	std::vector<Trace> channels_;

	float xMin_, xMax_;
	float yMin_, yMax_;
};

} // namespace

