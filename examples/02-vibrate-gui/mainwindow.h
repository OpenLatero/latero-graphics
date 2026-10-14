#pragma once

#include <gtkmm.h>
#include <laterographics/tactiledisplayview.h>
#include <laterographics/tactileengine.h>

class MainWindow : public Gtk::ApplicationWindow
{
public:
	MainWindow(const latero::Tactograph *dev, latero::graphics::GeneratorPtr gen);
	virtual ~MainWindow();

protected:
	latero::graphics::TactileDisplayView preview_;
};
