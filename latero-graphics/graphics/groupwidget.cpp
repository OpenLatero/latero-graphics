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

#include <filesystem>

#include "groupwidget.h"
#include "group.h"
#include "pattern.h"

#include "line.h"
#include "dot.h"
#include "dots.h"
#include "circle.h"
#include "polygon.h"
#include "image.h"
#include "texture/texturewidget.h"
#include "texture/texturedropdown.h"
#include "texture/texture.h"
#include "texture/doublelineargratingtexture.h"
#include "texture/motiontexture.h"
#include "patternpreview.h"
#include "dots.h"
#include "patterncreatordialog.h"
#include "modulator/modulatordropdown.h"
#include "modulator/modulator.h"

namespace latero::graphics {


GroupOpDropDown::GroupOpDropDown(GroupPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	list_(Gtk::StringList::create({})),
	dropDown_(list_),
	peer_(peer)
{
	for (const auto& op : peer->GetOperations())
		list_->append(op.label);
	Glib::ustring target = peer->GetOperation().label;
	for (guint i = 0; i < list_->get_n_items(); ++i)
		if (list_->get_string(i) == target) { dropDown_.set_selected(i); break; }
	dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &GroupOpDropDown::OnChange));
	append(dropDown_);
}

void GroupOpDropDown::OnChange() { peer_->SetOperation(std::string(list_->get_string(dropDown_.get_selected()))); signalChanged_(); }






GroupTreeView::GroupTreeView(GroupPtr peer) :
	peer_(peer),
	selection_(Gtk::SingleSelection::create())
{
	auto factory = Gtk::SignalListItemFactory::create();
	factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		auto expander = Gtk::make_managed<Gtk::TreeExpander>();
		expander->set_child(*Gtk::make_managed<Gtk::Label>("", Gtk::Align::START));
		item->set_child(*expander);
	});
	factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		auto row = std::dynamic_pointer_cast<Gtk::TreeListRow>(item->get_item());
		if (!row) return;
		auto expander = dynamic_cast<Gtk::TreeExpander*>(item->get_child());
		if (!expander) return;
		expander->set_list_row(row);
		auto patItem = std::dynamic_pointer_cast<PatternItem>(row->get_item());
		if (!patItem || !patItem->pattern) return;
		auto label = dynamic_cast<Gtk::Label*>(expander->get_child());
		if (label) label->set_label(patItem->pattern->GetName());
	});
	factory->signal_unbind().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
		if (auto* expander = dynamic_cast<Gtk::TreeExpander*>(item->get_child()))
			expander->set_list_row({});
	});

	selection_->set_autoselect(false);
	selection_->set_can_unselect(true);
	set_factory(factory);
	set_model(selection_);
	Refresh();

	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &GroupTreeView::OnClick));
	add_controller(gesture);
}

Glib::RefPtr<Gio::ListModel> GroupTreeView::CreateChildModel(const Glib::RefPtr<Glib::ObjectBase>& item)
{
	auto patItem = std::dynamic_pointer_cast<PatternItem>(item);
	if (!patItem) return {};
	GroupPtr group = boost::dynamic_pointer_cast<Group>(patItem->pattern);
	if (!group || !group->ChildrenArePublic()) return {};
	auto store = Gio::ListStore<PatternItem>::create();
	for (auto& child : group->GetPatterns())
		if (child) store->append(PatternItem::create(child));
	return store;
}

void GroupTreeView::RebuildMenu(PatternPtr pattern)
{
	GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
	bool has_parent = (bool)GetParentGroup(pattern);
	TexturePtr tx = boost::dynamic_pointer_cast<Texture>(pattern);

	// Build action group
	actionGroup_ = Gio::SimpleActionGroup::create();
	insert_action_group("tree", actionGroup_);

	// Build menu model
	auto menu = Gio::Menu::create();

	if (group && group->ChildrenArePublic())
	{
		if (tx)
		{
			actionGroup_->add_action("add_to_group", sigc::mem_fun(*this, &GroupTreeView::OnGroupAddTexture));
			menu->append("Add to group", "tree.add_to_group");
		}
		else
		{
			actionGroup_->add_action("add_to_group", sigc::mem_fun(*this, &GroupTreeView::OnGroupAddPattern));
			menu->append("Add to group", "tree.add_to_group");
		}
	}

	if (!tx)
		actionGroup_->add_action("save", sigc::mem_fun(*this, &GroupTreeView::OnPatternSave));
	else
		actionGroup_->add_action("save", sigc::mem_fun(*this, &GroupTreeView::OnTextureSave));
	menu->append("Save to file", "tree.save");

	if (has_parent)
	{
		actionGroup_->add_action("load",      sigc::mem_fun(*this, &GroupTreeView::OnPatternLoad));
		actionGroup_->add_action("remove",    sigc::mem_fun(*this, &GroupTreeView::OnPatternRemove));
		actionGroup_->add_action("append",    sigc::mem_fun(*this, &GroupTreeView::OnPatternAppend));
		actionGroup_->add_action("prepend",   sigc::mem_fun(*this, &GroupTreeView::OnPatternPrepend));
		actionGroup_->add_action("move_up",   sigc::mem_fun(*this, &GroupTreeView::OnPatternMoveUp));
		actionGroup_->add_action("move_down", sigc::mem_fun(*this, &GroupTreeView::OnPatternMoveDown));
		menu->append("Load from file", "tree.load");
		menu->append("Remove",         "tree.remove");
		menu->append("Add after",      "tree.append");
		menu->append("Add before",     "tree.prepend");
		menu->append("Move up",        "tree.move_up");
		menu->append("Move down",      "tree.move_down");
	}

	if (!popupMenu_) {
		popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu);
		popupMenu_->set_parent(*this);
	} else {
		popupMenu_->set_menu_model(menu);
	}
}


void GroupTreeView::OnClick(int n_press, double x, double y)
{
	PatternPtr pattern = GetCurrentPattern();
	if (pattern)
	{
		RebuildMenu(pattern);
		popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
		popupMenu_->popup();
	}
}

void GroupTreeView::OnPatternAppend()
{
	PatternPtr curPattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(curPattern);
	if (!curPattern || !parent) return;

	// TODO: try to reduce duplication of code below
	TexturePtr tx = boost::dynamic_pointer_cast<Texture>(parent);
	if (tx)
	{
		// TODO: Consider making CreateTextureDlg a PatternCreatorDialog (or derive both from the same class) so that we don't need to duplicate the code below. This is needed
		// so that we have a single method to call to create the texture/pattern based.
		auto dlg = new CreateTextureDlg(peer_->Dev());
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		{
			dlg->set_transient_for(*win);
			dlg->set_modal(true);
		}
		dlg->signal_response().connect([this, dlg, curPattern, parent](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
			{
				PatternPtr newPattern = dlg->CreateTexture();
				if (newPattern) { parent->InsertPatternAfter(newPattern, curPattern); Refresh(); Select(newPattern); }
			}
			delete dlg;
		});
		dlg->present();
	}
	else
	{
		auto dlg = new PatternCreatorDialog(peer_->Dev());
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		{
			dlg->set_transient_for(*win);
			dlg->set_modal(true);
		}
		dlg->signal_response().connect([this, dlg, curPattern, parent](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
			{
				PatternPtr newPattern = dlg->CreatePattern();
				if (newPattern) { parent->InsertPatternAfter(newPattern, curPattern); Refresh(); Select(newPattern); }
			}
			delete dlg;
		});
		std::cout << "---- HERE ----" << std::endl;
		dlg->present();
	}
}

void GroupTreeView::OnPatternPrepend()
{
	// TODO: share code with OnPatternAppend() to reduce duplication

	PatternPtr curPattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(curPattern);
	if (!curPattern || !parent) return;

	TexturePtr tx = boost::dynamic_pointer_cast<Texture>(parent);
	if (!tx)
	{
		auto dlg = new PatternCreatorDialog(peer_->Dev());
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		{
			dlg->set_transient_for(*win);
			dlg->set_modal(true);
		}
		dlg->signal_response().connect([this, dlg, curPattern, parent](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
			{
				PatternPtr newPattern = dlg->CreatePattern();
				if (newPattern) { parent->InsertPatternBefore(newPattern, curPattern); Refresh(); Select(newPattern); }
			}
			delete dlg;
		});
		dlg->present();
	}
	else
	{
		auto dlg = new CreateTextureDlg(peer_->Dev());
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		{
			dlg->set_transient_for(*win);
			dlg->set_modal(true);
		}
		dlg->signal_response().connect([this, dlg, curPattern, parent](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
			{
				PatternPtr newPattern = dlg->CreateTexture();
				if (newPattern) { parent->InsertPatternBefore(newPattern, curPattern); Refresh(); Select(newPattern); }
			}
			delete dlg;
		});
		dlg->present();
	}
}

void GroupTreeView::OnPatternMoveUp()
{
	PatternPtr curPattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(curPattern);
	if (curPattern && parent)
	{
		parent->MoveUpPattern(curPattern);
		Refresh();
		Select(curPattern);
	}
}

void GroupTreeView::OnPatternMoveDown()
{
	PatternPtr curPattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(curPattern);
	if (curPattern && parent)
	{
		parent->MoveDownPattern(curPattern);
		Refresh();
		Select(curPattern);
	}
}



PatternPtr GroupTreeView::GetCurrentPattern()
{
	auto row = std::dynamic_pointer_cast<Gtk::TreeListRow>(selection_->get_selected_item());
	if (!row) return PatternPtr();
	auto patItem = std::dynamic_pointer_cast<PatternItem>(row->get_item());
	return patItem ? patItem->pattern : PatternPtr();
}


void GroupTreeView::Refresh()
{
	auto rootStore = Gio::ListStore<PatternItem>::create();
	rootStore->append(PatternItem::create(peer_));
	treeModel_ = Gtk::TreeListModel::create(rootStore,
		sigc::mem_fun(*this, &GroupTreeView::CreateChildModel), false, true);
	refreshing_ = true;
	selection_->set_model(treeModel_);
	refreshing_ = false;
}


void GroupTreeView::Select(PatternPtr pattern)
{
	guint n = treeModel_->get_n_items();
	for (guint i = 0; i < n; ++i) {
		auto row = treeModel_->get_row(i);
		if (!row) continue;
		auto patItem = std::dynamic_pointer_cast<PatternItem>(row->get_item());
		if (patItem && patItem->pattern == pattern) {
			selection_->set_selected(i);
			return;
		}
	}
}


GroupPtr GroupTreeView::GetParentGroup(PatternPtr pattern)
{
	if (!pattern) return GroupPtr();
	guint n = treeModel_->get_n_items();
	for (guint i = 0; i < n; ++i) {
		auto row = treeModel_->get_row(i);
		if (!row) continue;
		auto patItem = std::dynamic_pointer_cast<PatternItem>(row->get_item());
		if (!patItem || patItem->pattern != pattern) continue;
		auto parentRow = row->get_parent();
		if (!parentRow) return GroupPtr();
		auto parentItem = std::dynamic_pointer_cast<PatternItem>(parentRow->get_item());
		if (!parentItem) return GroupPtr();
		return boost::dynamic_pointer_cast<Group>(parentItem->pattern);
	}
	return GroupPtr();
}

void GroupTreeView::SelectFirst()
{
	if (treeModel_ && treeModel_->get_n_items() > 0)
		selection_->set_selected(0);
}




void GroupTreeView::OnPatternRemove()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(pattern);
	if (pattern && parent)
	{
		parent->RemovePattern(pattern);
		Refresh();
		Select(parent);
	}
}

void GroupTreeView::OnGroupAddPattern()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
	if (!group) return;

	auto dlg = new PatternCreatorDialog(peer_->Dev());
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
	{
		dlg->set_transient_for(*win);
		dlg->set_modal(true);
	}
	dlg->signal_response().connect([this, dlg, group](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
		{
			PatternPtr obj = dlg->CreatePattern();
			group->InsertPattern(obj);
			Refresh();
			Select(obj);
		}
		delete dlg;
	});
	dlg->present();
}

void GroupTreeView::OnGroupAddTexture()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
	if (!group) return;

	auto dlg = new CreateTextureDlg(peer_->Dev());
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
	{
		dlg->set_transient_for(*win);
		dlg->set_modal(true);
	}
	dlg->signal_response().connect([this, dlg, group](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
		{
			PatternPtr obj = dlg->CreateTexture();
			group->InsertPattern(obj);
			Refresh();
			Select(obj);
		}
		delete dlg;
	});
	dlg->present();
}



void GroupTreeView::OnPatternSave()
{
	PatternPtr pattern = GetCurrentPattern();
	if (!pattern) return;

	std::string file = pattern->GetXMLFile();
	if (file == "") file = "new.pattern";
	file = std::filesystem::path(file).filename().string();

	auto filter = Gtk::FileFilter::create();
	filter->add_suffix("pattern");

	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select a file...");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_initial_name(file);
	dialog->set_default_filter(filter);

	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->save(*win, [pattern, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto f = dialog->save_finish(result);
pattern->SaveToFile(f->get_path());
		} catch (const Gtk::DialogError&) {}
	});
}


void GroupTreeView::OnTextureSave()
{
	PatternPtr pattern = GetCurrentPattern();
	if (!pattern) return;

	std::string file = pattern->GetXMLFile();
	if (file == "") file = "new.tx";
	file = std::filesystem::path(file).filename().string();

	auto filter = Gtk::FileFilter::create();
	filter->add_suffix("tx");

	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select a file...");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_initial_name(file);
	dialog->set_default_filter(filter);

	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->save(*win, [pattern, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto f = dialog->save_finish(result);
			pattern->SaveToFile(f->get_path());
		} catch (const Gtk::DialogError&) {}
	});
}


void GroupTreeView::OnPatternLoad()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(pattern);
	if (!pattern || !parent) return;

	auto filter = Gtk::FileFilter::create();
	filter->add_suffix("pattern");
	filter->add_suffix("tx");

	auto filters = Gio::ListStore<Gtk::FileFilter>::create();
	filters->append(filter);

	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select a file...");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_filters(filters);

	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->open(*win, [this, pattern, parent, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto f = dialog->open_finish(result);
			PatternPtr newPattern = Pattern::Create(pattern->Dev(), f->get_path());
			if (!newPattern) return;
			parent->ReplacePattern(pattern, newPattern);
			Refresh();
			Select(newPattern);
		} catch (const Gtk::DialogError&) {}
	});
}


class GroupPanel : public Gtk::Box
{
public:
	GroupPanel(GroupPtr peer) : Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
	{
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		auto opDropDown = Gtk::make_managed<GroupOpDropDown>(peer);
		auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

		box->append(*opDropDown);
		box->append(opWidgetHolder_);
		opWidgetHolder_.set_hexpand();
		OnOpChanged();

		box->set_hexpand();
		patternPreview->set_hexpand(false);
		patternPreview->set_size_request(200, -1);

		append(*box);
		append(*patternPreview);

		opDropDown->SignalChanged().connect(
			sigc::mem_fun(*this, &GroupPanel::OnOpChanged));
	}

	void OnOpChanged()
	{
		// clear op widget
		opWidgetHolder_.unset_child();

		if (peer_->GetOperation() == Group::op_reactive)
		{
			ModulatorPtr mod = peer_->GetReactiveMod();
			if (mod)
			{
				auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
				opWidgetHolder_.set_child(*box);

				auto modDropDown = Gtk::make_managed<ModulatorDropDown>(mod);

				box->append(*modDropDown);
				box->append(modWidgetHolder_);
				modWidgetHolder_.set_hexpand();


				modWidgetHolder_.unset_child();
				modWidgetHolder_.set_child(*Gtk::manage(mod->CreateWidget(mod)));

				modDropDown->SignalModulatorChanged().connect(
					sigc::mem_fun(*this, &GroupPanel::OnModulatorChanged));
			}
		}
	}

	void OnModulatorChanged(ModulatorPtr mod)
	{
		peer_->SetReactiveMod(mod);

		modWidgetHolder_.unset_child();
		modWidgetHolder_.set_child(*Gtk::manage(mod->CreateWidget(mod)));
	}

	virtual ~GroupPanel() {}

protected:
	Gtk::Frame opWidgetHolder_;
	Gtk::Frame modWidgetHolder_;
	GroupPtr peer_;
};


class ComboTexturePanel : public Gtk::Box
{
public:
	ComboTexturePanel(ComboTexturePtr peer, GroupTreeView *treeView, bool showSelector) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), treeView_(treeView), txCtrl_(peer), peer_(peer)
	{
		auto textureAmplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);

		textureAmplitudeCtrl->set_vexpand();

		auto lbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		lbox->append(*Gtk::make_managed<TextureInvertCtrl>(peer));
		lbox->append(*textureAmplitudeCtrl);

		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		box->append(*Gtk::make_managed<GroupOpDropDown>(peer));
		box->append(*Gtk::make_managed<OscillatorWidget>(peer->GetOscillator(),true));

		if (showSelector)
		{
			append(txCtrl_);
			txCtrl_.SignalTextureChanged().connect(sigc::mem_fun(*this, &ComboTexturePanel::OnTextureChange));	
		}

		append(*lbox);
		append(*box);
		box->set_hexpand();
		append(*Gtk::make_managed<PatternPreview>(peer));
	}

	virtual ~ComboTexturePanel() {}

	void OnTextureChange()
	{
		TexturePtr newTx = txCtrl_.GetTexture();
		GroupPtr group = treeView_->GetParentGroup(peer_);
		if (group) group->ReplacePattern(peer_, newTx);
		//peer_ = newTx;
		treeView_->Refresh();
		treeView_->Select(newTx);
		// TODO: I have a bad feeling about this... the object probably gets destroyed when Refresh is called...
	}

protected:
    GroupTreeView *treeView_;
	TextureSelectorCtrl txCtrl_;
	ComboTexturePtr peer_;
};



GroupWidget::GroupWidget(GroupPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer), treeView_(peer), txWidget_(NULL)
{
    // this was in a vbox above the tree view but commented out, not sure why
    //pSideBar->append(*Gtk::make_managed<GroupOpCombo>(peer));
    
	Gtk::ScrolledWindow *scrolledTreeView = Gtk::make_managed<Gtk::ScrolledWindow>();
    scrolledTreeView->set_size_request(200,200);
	scrolledTreeView->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	scrolledTreeView->set_placement(Gtk::CornerType::TOP_RIGHT);
	scrolledTreeView->set_child(treeView_);
    append(*scrolledTreeView);
    
    append(objWidgetHolder_);
	objWidgetHolder_.set_hexpand();

	treeView_.selection_->property_selected_item().signal_changed().connect(
		sigc::mem_fun(*this, &GroupWidget::OnSelectionChanged));
	treeView_.SelectFirst();
}


void GroupWidget::OnTextureChange()
{
	if (!txWidget_) return;
	if (!currentTx_) return;

	TexturePtr newTx = txWidget_->GetTexture();
	GroupPtr group = treeView_.GetParentGroup(currentTx_);
	if (group)
		group->ReplacePattern(currentTx_, newTx);

	currentTx_ = newTx;
	treeView_.Refresh();
	treeView_.Select(newTx);
}


void GroupWidget::OnSelectionChanged()
{
	if (treeView_.refreshing_) return;
	objWidgetHolder_.unset_child();

	txWidget_ = NULL;
	currentTx_ = TexturePtr();

	PatternPtr pattern = treeView_.GetCurrentPattern();
	if (pattern)
	{
		GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
		TexturePtr tx = boost::dynamic_pointer_cast<Texture>(pattern);
		ComboTexturePtr combotx = boost::dynamic_pointer_cast<ComboTexture>(pattern);
		DotsPtr dots = boost::dynamic_pointer_cast<Dots>(pattern);
		bool hasParent = (bool)(treeView_.GetParentGroup(pattern));

		if (group)
		{
			if (boost::dynamic_pointer_cast<DoubleLinearGratingTexture>(tx) ||
			    boost::dynamic_pointer_cast<MotionTexture>(tx))
			{
				txWidget_ = Gtk::make_managed<TextureSelectorWidget>(tx);
				currentTx_ = tx;
				objWidgetHolder_.set_child(*txWidget_);
				txWidget_->SignalTextureChanged().connect(sigc::mem_fun(*this, &GroupWidget::OnTextureChange));
			}
			else if (combotx)
			{
				objWidgetHolder_.set_child(*Gtk::make_managed<ComboTexturePanel>(combotx,&treeView_,hasParent));
			}
			else if (dots)
			{
				objWidgetHolder_.set_child(*Gtk::manage(dots->CreateWidget(dots)));
			}
			else
			{
				objWidgetHolder_.set_child(*Gtk::make_managed<GroupPanel>(group));
			}
		}
		else if (tx)
		{
			txWidget_ = Gtk::make_managed<TextureSelectorWidget>(tx);
			currentTx_ = tx;
			objWidgetHolder_.set_child(*txWidget_);
			txWidget_->SignalTextureChanged().connect(sigc::mem_fun(*this, &GroupWidget::OnTextureChange));
		}
		else
		{
			objWidgetHolder_.set_child(*Gtk::manage(pattern->CreateWidget(pattern)));
		}
	}
}

} // namespace

