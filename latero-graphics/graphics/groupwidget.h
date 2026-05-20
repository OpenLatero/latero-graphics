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

#pragma once

#include "patternfwd.h"
#include "groupfwd.h"
#include <gtkmm.h>
#include "texture/textureselectorwidget.h"

namespace latero::graphics {

class GroupOpDropDown : public Gtk::Box
{
public:
	GroupOpDropDown(GroupPtr peer);
	virtual ~GroupOpDropDown() {};
	sigc::signal<void()>& SignalChanged() { return signalChanged_; };
private:
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	sigc::signal<void()> signalChanged_;
	void OnChange();
	GroupPtr peer_;
};

class PatternItem : public Glib::Object
{
public:
	PatternPtr pattern;
	static Glib::RefPtr<PatternItem> create(PatternPtr p) {
		return Glib::make_refptr_for_instance<PatternItem>(new PatternItem(p));
	}
protected:
	explicit PatternItem(PatternPtr p) : Glib::ObjectBase(typeid(PatternItem)), pattern(p) {}
};

class GroupTreeView : public Gtk::ListView
{
public:
	GroupTreeView(GroupPtr peer);
	virtual ~GroupTreeView() { if (popupMenu_) popupMenu_->unparent(); }

	void Select(PatternPtr pattern);
	void SelectFirst();
	void Refresh();
	GroupPtr GetParentGroup(PatternPtr pattern);

	PatternPtr GetCurrentPattern();
	void OnClick(int n_press, double x, double y);
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

	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	Glib::RefPtr<Gio::SimpleActionGroup> actionGroup_;
	Glib::RefPtr<Gtk::SingleSelection> selection_;
	Glib::RefPtr<Gtk::TreeListModel> treeModel_;
	GroupPtr peer_;
	bool refreshing_ = false;

private:
	Glib::RefPtr<Gio::ListModel> CreateChildModel(const Glib::RefPtr<Glib::ObjectBase>& item);
};


class GroupWidget : public Gtk::Box
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

} // namespace

