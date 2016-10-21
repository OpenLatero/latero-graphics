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

#ifndef LATERO_GRAPHICS_PLANAR_OBJECT_SET_WIDGET
#define LATERO_GRAPHICS_PLANAR_OBJECT_SET_WIDGET

#include "patternfwd.h"
#include "groupfwd.h"
#include <gtkmm.h>
#include "texture/textureselectorwidget.h"

namespace latero {
namespace graphics { 

class GroupOpCombo : public Gtk::ComboBoxText
{
public:
	GroupOpCombo(GroupPtr peer);
	virtual ~GroupOpCombo() {};
	sigc::signal<void> SignalChanged() { return signalChanged_; };
private:
	sigc::signal<void> signalChanged_;
	void OnChange();
	GroupPtr peer_;
};

class GroupTreeView : public Gtk::TreeView
{
public:
	GroupTreeView(GroupPtr peer);
	virtual ~GroupTreeView() {}

	class ListColumns : public Gtk::TreeModelColumnRecord
	{
	public:
		ListColumns() { add(name_); add(obj_); }
		Gtk::TreeModelColumn<std::string> name_;
		Gtk::TreeModelColumn<PatternPtr> obj_;
	};

	void Select(PatternPtr pattern);
	void SelectFirst();
	void Refresh();
	void InsertPattern(PatternPtr pattern, Gtk::TreeModel::Row* row, Glib::RefPtr<Gtk::TreeStore> store);
	Gtk::TreeModel::Children::iterator GetIter(PatternPtr pattern, Gtk::TreeModel::Children children);
	GroupPtr GetParentGroup(PatternPtr pattern);

	PatternPtr GetCurrentPattern();
	virtual bool on_button_press_event(GdkEventButton* event);
	void OnPatternRemove();
	void OnPatternSave();
	void OnTextureSave();
	void OnPatternLoad();
	void OnPatternAppend();
	void OnPatternPrepend();
	void OnPatternMoveUp();
	void OnPatternMoveDown();
	void OnGroupAddPattern();
	void OnGroupAddTexture();

	void RebuildMenu(PatternPtr pattern);

	Gtk::Menu menu_;
	ListColumns columns_;
	GroupPtr peer_;
};


class GroupWidget : public Gtk::HBox
{
protected:

public:
	GroupWidget(GroupPtr peer);
	virtual ~GroupWidget() {};

protected:
	void OnSelectionChanged();
	void OnTextureChange();

	GroupPtr peer_;
	GroupTreeView treeView_;
	Gtk::Frame objWidgetHolder_;

	// hack
	TexturePtr currentTx_;
	TextureSelectorWidget *txWidget_;
};

} // namespace graphics
} // namespace latero

#endif
