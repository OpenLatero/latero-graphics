#pragma once

#include <gtkmm.h>
#include "patterntexturefwd.h"

namespace latero::graphics {

class PatternTextureWidget : public Gtk::Box
{
public:
	PatternTextureWidget(PatternTexturePtr peer);
	virtual ~PatternTextureWidget() {}

protected:
	void OnLoad();
	PatternTexturePtr peer_;
	Gtk::Frame patternHolder_;
	
};

class PatternTextureAdvancedWidget : public Gtk::Box
{
public:
	PatternTextureAdvancedWidget(PatternTexturePtr peer);
	virtual ~PatternTextureAdvancedWidget() {}

protected:
	void OnLoad();
	PatternTexturePtr peer_;
	Gtk::Frame patternHolder_;
	
};

} // namespace

