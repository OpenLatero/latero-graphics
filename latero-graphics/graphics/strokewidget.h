#pragma once

#include "strokemotionwidget.h"

#include "strokefwd.h"
#include <gtkmm.h>


namespace latero::graphics {

/**
 * Profile of the stroke across the line.
 */
class StrokeProfileWidget : public Gtk::Box
{
public:
	StrokeProfileWidget(StrokePtr peer);
	virtual ~StrokeProfileWidget() {};
};


/**
 * Fill texture
 */
class StrokeFillWidget : public Gtk::Box
{
public:
	StrokeFillWidget(StrokePtr peer);
	virtual ~StrokeFillWidget() {};
};


/**
 * Dotted texture
 */
class StrokeDottedWidget : public Gtk::Box
{
public:
	StrokeDottedWidget(StrokePtr peer);
	virtual ~StrokeDottedWidget() {};
};

} // namespace

