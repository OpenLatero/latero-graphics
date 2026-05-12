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
#include "texture/texturecombo.h"
#include "texture/texture.h"
#include "texture/doublelineargratingtexture.h"
#include "texture/motiontexture.h"
#include "patternpreview.h"
#include "dots.h"
#include "patterncreatordialog.h"
#include "modulator/modulatorcombo.h"
#include "modulator/modulator.h"

namespace latero {
namespace graphics { 


GroupOpCombo::GroupOpCombo(GroupPtr peer) :
	peer_(peer)
{
	Group::OperationSet ops = peer->GetOperations();
	for (unsigned int i=0; i<ops.size(); ++i)
		append(ops[i].label);
	set_active_text(peer->GetOperation().label);
	signal_changed().connect(sigc::mem_fun(*this, &GroupOpCombo::OnChange));
}
void GroupOpCombo::OnChange() { peer_->SetOperation(get_active_text()); signalChanged_(); }






GroupTreeView::GroupTreeView(GroupPtr peer) :
	peer_(peer)
{
	Refresh();
	append_column("name", columns_.name_);
	set_headers_visible (false);

	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &GroupTreeView::OnClick));
	add_controller(gesture);
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
		dlg->show();
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
		dlg->show();
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
		dlg->show();
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
		dlg->show();
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
	Gtk::TreeModel::iterator iter = get_selection()->get_selected();
	if (iter)
		return (*iter)[columns_.obj_];
	else
		return PatternPtr();
}



void GroupTreeView::Refresh()
{
	Glib::RefPtr<Gtk::TreeStore> store = Gtk::TreeStore::create(columns_);
	Gtk::TreeModel::Row row = *store->append();
	InsertPattern(peer_, &row, store);
	set_model(store);
	expand_all();
}


void GroupTreeView::InsertPattern(PatternPtr pattern, Gtk::TreeModel::Row* row, Glib::RefPtr<Gtk::TreeStore> store)
{
	(*row)[columns_.name_] = pattern->GetName();
	(*row)[columns_.obj_] = pattern;

	GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
	if (!group) return;

	if (group->ChildrenArePublic())
	{
		std::vector<PatternPtr> objects = group->GetPatterns();
		for (unsigned int i=0; i<objects.size(); ++i)
		{
			Gtk::TreeModel::Row entry = *store->append(row->children());
			InsertPattern(objects[i], &entry, store);
		}
	}
}


void GroupTreeView::Select(PatternPtr pattern)
{
	Gtk::TreeModel::Children::iterator iter = GetIter(pattern, get_model()->children());
	if (iter) get_selection()->select(iter);
}


Gtk::TreeModel::Children::iterator GroupTreeView::GetIter(PatternPtr pattern, Gtk::TreeModel::Children children)
{
	typedef Gtk::TreeModel::Children::iterator iterator;
	for (iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		PatternPtr curPattern = row[columns_.obj_];
		if (curPattern == pattern)
			return iter;

		iterator rv = GetIter(pattern, row.children());
		if (rv) return rv;
	}
	return iterator();
}


GroupPtr GroupTreeView::GetParentGroup(PatternPtr pattern)
{
	if (!pattern) return GroupPtr();

	Gtk::TreeModel::Children::iterator iter = GetIter(pattern, get_model()->children());
	if (!iter) return GroupPtr();
	
	Gtk::TreeModel::Children::iterator parentIter = (*iter).parent();
	if (!parentIter) return GroupPtr();

	PatternPtr parent = (*parentIter)[columns_.obj_];
	if (!parent) return GroupPtr();

	return boost::dynamic_pointer_cast<Group>(parent);
}

void GroupTreeView::SelectFirst()
{
    auto model = get_model();
    if (!model || model->children().empty())
        return;

	auto iter = get_model()->children().begin();
	if (iter) get_selection()->select(iter);
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
	dlg->show();
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
	dlg->show();
}



void GroupTreeView::OnPatternSave()
{
	PatternPtr pattern = GetCurrentPattern();
	if (!pattern) return;

	auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::SAVE);
	auto filter = Gtk::FileFilter::create();
	filter->add_pattern("*.pattern");
	dialog->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("Save", Gtk::ResponseType::OK);
	dialog->set_default_response(Gtk::ResponseType::CANCEL);
	std::string file = pattern->GetXMLFile();
	if (file=="") file = "new.pattern";
	dialog->set_current_name(file);
	dialog->add_filter(filter);
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
	{
		dialog->set_transient_for(*win);
		dialog->set_modal(true);
	}
	dialog->signal_response().connect([pattern, dialog](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
			pattern->SaveToFile(dialog->get_file()->get_path());
		delete dialog;
	});
	dialog->show();
}


void GroupTreeView::OnTextureSave()
{
	PatternPtr pattern = GetCurrentPattern();
	if (!pattern) return;

	auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::SAVE);
	auto filter = Gtk::FileFilter::create();
	filter->add_pattern("*.tx");
	dialog->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("Save", Gtk::ResponseType::OK);
	dialog->set_default_response(Gtk::ResponseType::CANCEL);
	std::string file = pattern->GetXMLFile();
	if (file=="") file = "new.tx";
	dialog->set_current_name(file);
	dialog->add_filter(filter);
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
	{
		dialog->set_transient_for(*win);
		dialog->set_modal(true);
	}
	dialog->signal_response().connect([pattern, dialog](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
			pattern->SaveToFile(dialog->get_file()->get_path());
		delete dialog;
	});
	dialog->show();
}


void GroupTreeView::OnPatternLoad()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(pattern);
	if (!pattern || !parent) return;

	auto dialog = new Gtk::FileChooserDialog("Please select a file...", Gtk::FileChooser::Action::OPEN);
	auto filter = Gtk::FileFilter::create();
	filter->add_pattern("*.pattern");
	filter->add_pattern("*.tx");
	dialog->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("Open", Gtk::ResponseType::OK);
	dialog->set_default_response(Gtk::ResponseType::CANCEL);
	std::string file = pattern->GetXMLFile();
	if (file=="") file = "new.pattern";
	dialog->set_current_name(file);
	dialog->add_filter(filter);
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
	{
		dialog->set_transient_for(*win);
		dialog->set_modal(true);
	}
	dialog->signal_response().connect([this, pattern, parent, dialog](int response_id) {
		if (response_id == Gtk::ResponseType::OK)
		{
			PatternPtr newPattern = Pattern::Create(pattern->Dev(), dialog->get_file()->get_path());
			parent->ReplacePattern(pattern, newPattern);
			Refresh();
			Select(newPattern);
		}
		delete dialog;
	});
	dialog->show();
}


class GroupPanel : public Gtk::Box
{
public:
	GroupPanel(GroupPtr peer) : Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
	{
		auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
		auto opCombo = Gtk::manage(new GroupOpCombo(peer));
		auto patternPreview = Gtk::manage(new PatternPreview(peer));

		box->append(*opCombo);
		box->append(opWidgetHolder_);
		opWidgetHolder_.set_hexpand();
		OnOpChanged();

		box->set_hexpand();
		patternPreview->set_hexpand(false);
		patternPreview->set_size_request(200, -1);

		append(*box);
		append(*patternPreview);

		opCombo->SignalChanged().connect(
			sigc::mem_fun(*this, &GroupPanel::OnOpChanged));
	}

	void OnOpChanged()
	{
		// clear op widget
		Widget *old = opWidgetHolder_.get_child();
		opWidgetHolder_.unset_child();
		delete old;

		if (peer_->GetOperation() == Group::op_reactive)
		{
			ModulatorPtr mod = peer_->GetReactiveMod();
			if (mod)
			{
				Gtk::Box *box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
				opWidgetHolder_.set_child(*box);

				ModulatorCombo *modCombo = Gtk::manage(new ModulatorCombo(mod));

				box->append(*modCombo);
				box->append(modWidgetHolder_);
				modWidgetHolder_.set_hexpand();


				Widget *old = modWidgetHolder_.get_child();
				modWidgetHolder_.unset_child();
				delete old;
				modWidgetHolder_.set_child(*Gtk::manage(mod->CreateWidget(mod)));

				modCombo->SignalModulatorChanged().connect(
					sigc::mem_fun(*this, &GroupPanel::OnModulatorChanged));
			}
		}
	}

	void OnModulatorChanged(ModulatorPtr mod)
	{
		peer_->SetReactiveMod(mod);

		Widget *old = modWidgetHolder_.get_child();
		modWidgetHolder_.unset_child();
		delete old;

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
		auto textureAmplitudeCtrl = Gtk::manage(new TextureAmplitudeCtrl(peer));

		textureAmplitudeCtrl->set_vexpand();

		auto lbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
		lbox->append(*Gtk::manage(new TextureInvertCtrl(peer)));
		lbox->append(*textureAmplitudeCtrl);

		auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
		box->append(*Gtk::manage(new GroupOpCombo(peer)));
		box->append(*Gtk::manage(new OscillatorWidget(peer->GetOscillator(),true)));

		if (showSelector)
		{
			append(txCtrl_);
			txCtrl_.SignalTextureChanged().connect(sigc::mem_fun(*this, &ComboTexturePanel::OnTextureChange));	
		}

		append(*lbox);
		append(*box);
		box->set_hexpand();
		append(*Gtk::manage(new PatternPreview(peer)));
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
    //pSideBar->append(*Gtk::manage(new GroupOpCombo(peer)));
    
	Gtk::ScrolledWindow *scrolledTreeView = Gtk::manage(new Gtk::ScrolledWindow);
    scrolledTreeView->set_size_request(200,200);
	scrolledTreeView->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	scrolledTreeView->set_placement(Gtk::CornerType::TOP_RIGHT);
	scrolledTreeView->set_child(treeView_);
    append(*scrolledTreeView);
    
    append(objWidgetHolder_);
	objWidgetHolder_.set_hexpand();

	treeView_.get_selection()->signal_changed().connect(
    		sigc::mem_fun(*this, &GroupWidget::OnSelectionChanged));
    
	// select the first item
	auto iter = treeView_.get_model()->children().begin();
	if (iter) treeView_.get_selection()->select(iter);
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
				txWidget_ = Gtk::manage(new TextureSelectorWidget(tx));
				currentTx_ = tx;
				objWidgetHolder_.set_child(*txWidget_);
				txWidget_->SignalTextureChanged().connect(sigc::mem_fun(*this, &GroupWidget::OnTextureChange));
			}
			else if (combotx)
			{
				objWidgetHolder_.set_child(*Gtk::manage(new ComboTexturePanel(combotx,&treeView_,hasParent)));
			}
			else if (dots)
			{
				objWidgetHolder_.set_child(*Gtk::manage(dots->CreateWidget(dots)));
			}
			else
			{
				objWidgetHolder_.set_child(*Gtk::manage(new GroupPanel(group)));
			}
		}
		else if (tx)
		{
			txWidget_ = Gtk::manage(new TextureSelectorWidget(tx));
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

} // namespace graphics
} // namespace latero

