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

#ifndef LATERO_GRAPHICS_GRATING_WIDGET
#define LATERO_GRAPHICS_GRATING_WIDGET

#include <gtkmm/dialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/checkbutton.h>
#include "gratingfwd.h"
#include "gratinggraph.h"
#include "gratingmodulatorwidget.h"
#include "gtk/checkframe.h"

namespace latero {
namespace graphics { 


/**
 * Invert
 */
class GratingInvertCtrl : public Gtk::HBox
{
public:
	GratingInvertCtrl(GratingPtr peer);
	virtual ~GratingInvertCtrl() {};
protected:
	void OnClick();
	Gtk::CheckButton check_;
	GratingPtr peer_;
};

/**
 * Enable
 */
class GratingEnableCtrl : public Gtk::HBox
{
public:
	GratingEnableCtrl(GratingPtr peer);
	virtual ~GratingEnableCtrl() {};
	sigc::signal<void> SignalClicked() { return signalClick_; };
protected:
	sigc::signal<void> signalClick_;
	void OnClick();
	Gtk::CheckButton check_;
	GratingPtr peer_;
};

/**
 * Regularization
 */
class GratingRegularizeButton : public Gtk::Button
{
public:
	GratingRegularizeButton(GratingPtr peer);
	virtual ~GratingRegularizeButton() {};
protected:
	virtual void on_clicked();
	GratingPtr peer_;
};

/**
 * Rhythm
 */
class GratingRhythmWidget : public Gtk::Frame
{
public:
	GratingRhythmWidget(GratingPtr peer);
	virtual ~GratingRhythmWidget();
protected:
	void OnAddCycle();
	void OnRemoveCycle();

	void AppendCycle(double value);
	void RemoveCycle();

	Gtk::VBox buttonBox_;
	Gtk::HBox sliderBox_;
	std::vector<Gtk::Adjustment*> adj_;
	std::vector<Gtk::Widget*> widget_;
	void OnChanged();
	GratingPtr peer_;
};



/**
 * GapSize
 */
class GratingGapSizeWidget : public Gtk::HBox
{
public:
	GratingGapSizeWidget(GratingPtr peer);
	virtual ~GratingGapSizeWidget() {}
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	GratingPtr peer_;
};

/**
 * RidgeSize
 */
class GratingRidgeSizeWidget : public Gtk::HBox
{
public:
	GratingRidgeSizeWidget(GratingPtr peer);
	virtual ~GratingRidgeSizeWidget() {}
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	GratingPtr peer_;
};


/**
 * Velocity
 */
class GratingVelocityWidget : public Gtk::HBox
{
public:
	GratingVelocityWidget(GratingPtr peer);
	virtual ~GratingVelocityWidget() {}
protected:
	void OnChangedUnits(std::string units);
	void OnChangedDist();
	void OnChangedHz();
	Gtk::Adjustment distAdj_, hzAdj_;
	GratingPtr peer_;
};


/**
 * Center
 */
class GratingCenterWidget : public Gtk::HBox
{
public:
	GratingCenterWidget(GratingPtr peer);
	virtual ~GratingCenterWidget() {}
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	GratingPtr peer_;
};

/**
 * Interp
 */
class GratingInterpWidget : public Gtk::Frame
{
public:
	GratingInterpWidget(GratingPtr peer);
	virtual ~GratingInterpWidget() {}
protected:
	void OnChanged();
	Gtk::RadioButton linearRadio_, arcRadio_;
	GratingPtr peer_;
};

/**
 * Cycle
 */
class GratingCycleWidget : public Gtk::HBox
{
public:
	GratingCycleWidget(GratingPtr peer);
	virtual ~GratingCycleWidget() {}
protected:
	GratingInterpWidget interpWidget_;
	GratingCenterWidget centerWidget_;
	GratingGraph graph_;
	GratingPtr peer_;
};

/**
 * Amplitude
 */
class GratingAmplitudeWidget : public Gtk::HBox
{
public:
	GratingAmplitudeWidget(GratingPtr peer);
	virtual ~GratingAmplitudeWidget() {}
protected:
	void OnChanged();
	Gtk::Adjustment adj_;
	GratingPtr peer_;
};


class AdvancedGratingWidget : public Gtk::Dialog
{
public:
	AdvancedGratingWidget(GratingPtr peer);
	virtual ~AdvancedGratingWidget() {};
};


class GratingAdvancedButton : public Gtk::Button
{
public:
	GratingAdvancedButton(GratingPtr peer);
	virtual ~GratingAdvancedButton() {};
protected:
	AdvancedGratingWidget dialog_;
	void OnPush();
	GratingPtr peer_;
};

class GratingPitchWidget : public Gtk::HBox
{
public:
	GratingPitchWidget(GratingPtr peer);
	virtual ~GratingPitchWidget() {} 
protected:
	void OnRegularize();
	Gtk::Widget *ridgeWidget_, *gapWidget_;
	GratingRegularizeButton regularizeButton_;
	GratingPtr peer_;
};


/**
 * Collection of interconnected grating widgets
 * @todo add more stuff here
 */
class GratingWidgetSet
{
public:
	GratingWidgetSet(GratingPtr peer);
	virtual ~GratingWidgetSet() {};
	void OnEnable();

	GratingEnableCtrl enableCtrl_;
	GratingVelocityWidget velocityWidget_;
	GratingPitchWidget pitchWidget_;
	GratingAdvancedButton advButton_;
	GratingInvertCtrl invertCtrl_;
	GratingModulatorCtrl compressCtrl_;
	GratingPtr peer_;
};

class GratingWidget : public gtk::CheckFrame
{
public:
	GratingWidget(GratingPtr peer);
	virtual ~GratingWidget() {};
protected:
	void OnEnable();
	GratingPtr peer_;
};

} // namespace graphics
} // namespace latero

#endif
