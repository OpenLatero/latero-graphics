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
#include <gtkmm/filechooserdialog.h>
#include "modulator/modulatorcombo.h"
#include "modulator/modulator.h"

namespace latero {
namespace graphics { 


GroupOpCombo::GroupOpCombo(GroupPtr peer) :
	peer_(peer)
{
	Group::OperationSet ops = peer->GetOperations();
	for (unsigned int i=0; i<ops.size(); ++i)
		append_text(ops[i].label);
	set_active_text(peer->GetOperation().label);
	signal_changed().connect(sigc::mem_fun(*this, &GroupOpCombo::OnChange));
}
void GroupOpCombo::OnChange() { peer_->SetOperation(get_active_text()); signalChanged_(); }






GroupTreeView::GroupTreeView(GroupPtr peer) :
	peer_(peer)
{
	set_size_request(200,200);	

	Refresh();
	append_column("name", columns_.name_);
	set_headers_visible (false);
}

void GroupTreeView::RebuildMenu(PatternPtr pattern)
{
	GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
	bool has_parent = (bool)etParentGroup(pattern);
	TexturePtr tx = boost::dynamic_pointer_cast<Texture>(pattern);
	
	menu_.items().erase(menu_.items().begin(),menu_.items().end());
	Gtk::Menu::MenuList& list = menu_.items();

	if (group) 
	{
		if (group->ChildrenArePublic())
		{
			if (tx)
				list.push_back( Gtk::Menu_Helpers::MenuElem("Add to group",
					sigc::mem_fun(*this, &GroupTreeView::OnGroupAddTexture) ) );
			else
				list.push_back( Gtk::Menu_Helpers::MenuElem("Add to group",
					sigc::mem_fun(*this, &GroupTreeView::OnGroupAddPattern) ) );
		}
	}

	if (!tx)
		list.push_back( Gtk::Menu_Helpers::MenuElem("Save to file",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternSave) ) );
	else
		list.push_back( Gtk::Menu_Helpers::MenuElem("Save to file",
			sigc::mem_fun(*this, &GroupTreeView::OnTextureSave) ) );


	if (has_parent)
	{
		list.push_back( Gtk::Menu_Helpers::MenuElem("_Load from file",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternLoad ) ) );

		list.push_back( Gtk::Menu_Helpers::MenuElem("_Remove",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternRemove) ) );

		list.push_back( Gtk::Menu_Helpers::MenuElem("_Add after",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternAppend ) ) );

		list.push_back( Gtk::Menu_Helpers::MenuElem("_Add before",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternPrepend ) ) );

		list.push_back( Gtk::Menu_Helpers::MenuElem("Move _up",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternMoveUp) ) );

		list.push_back( Gtk::Menu_Helpers::MenuElem("Move _down",
			sigc::mem_fun(*this, &GroupTreeView::OnPatternMoveDown) ) );
	}
}


bool GroupTreeView::on_button_press_event(GdkEventButton* event)
{
	bool rv = TreeView::on_button_press_event(event);
	if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
	{
		PatternPtr pattern = GetCurrentPattern();
		if (pattern)
		{
			RebuildMenu(pattern);
			menu_.popup(event->button, event->time);
		}
	}
	return rv;
}

void GroupTreeView::OnPatternAppend()
{
	PatternPtr curPattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(curPattern);
	if (curPattern && parent)
	{
		TexturePtr tx = boost::dynamic_pointer_cast<Texture>(parent);
		if (tx)
		{
			CreateTextureDlg dlg(peer_->Dev());
			if (Gtk::RESPONSE_OK == dlg.run())
			{
				PatternPtr newPattern = dlg.CreateTexture();
				if (newPattern)
				{
					parent->InsertPatternAfter(newPattern, curPattern);
					Refresh();
					Select(newPattern);
				}
			}
		}
		else
		{
			PatternCreatorDialog dlg(peer_->Dev());
			if (Gtk::RESPONSE_OK == dlg.run())
			{
				PatternPtr newPattern = dlg.CreatePattern();
				if (newPattern)
				{
					parent->InsertPatternAfter(newPattern, curPattern);
					Refresh();
					Select(newPattern);
				}
			}
		}
	}
}

void GroupTreeView::OnPatternPrepend()
{
	PatternPtr curPattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(curPattern);
	if (curPattern && parent)
	{
		TexturePtr tx = boost::dynamic_pointer_cast<Texture>(parent);
		if (!tx)
		{
			PatternCreatorDialog dlg(peer_->Dev());
			if (Gtk::RESPONSE_OK == dlg.run())
			{
				PatternPtr newPattern = dlg.CreatePattern();
				if (newPattern)
				{
					parent->InsertPatternBefore(newPattern, curPattern);
					Refresh();
					Select(newPattern);
				}
			}
		}
		else
		{
			CreateTextureDlg dlg(peer_->Dev());
			if (Gtk::RESPONSE_OK == dlg.run())
			{
				PatternPtr newPattern = dlg.CreateTexture();
				if (newPattern)
				{
					parent->InsertPatternBefore(newPattern, curPattern);
					Refresh();
					Select(newPattern);
				}
			}
		}
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
	show_all_children();
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

		iterator rv = GetIter(pattern, row->children());
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
	if (get_model()->children()<=0) return;
	Gtk::TreeModel::Row row = get_model()->children()[0];
	if (row) get_selection()->select(row);
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

	PatternCreatorDialog dlg(peer_->Dev());
	if (Gtk::RESPONSE_OK == dlg.run())
	{
		PatternPtr obj = dlg.CreatePattern();
		group->InsertPattern(obj);
		Refresh();
		Select(obj);
	}
}

void GroupTreeView::OnGroupAddTexture()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr group = boost::dynamic_pointer_cast<Group>(pattern);
	if (!group) return;

	CreateTextureDlg dlg(peer_->Dev());
	if (Gtk::RESPONSE_OK == dlg.run())
	{
		PatternPtr obj = dlg.CreateTexture();
		group->InsertPattern(obj);
		Refresh();
		Select(obj);
	}
}



void GroupTreeView::OnPatternSave()
{
	PatternPtr pattern = GetCurrentPattern();
	if (pattern)
	{
		Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_SAVE);

		std::string dir = Glib::get_current_dir();
 
		Gtk::FileFilter filter;
		filter.add_pattern("*.pattern");

		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_CANCEL);
		std::string file = pattern->GetXMLFile();
		if (file=="") file = "new.pattern";
		dialog.set_current_name(file);
		dialog.add_filter(filter);

		if (Gtk::RESPONSE_OK == dialog.run())
			pattern->SaveToFile(dialog.get_filename());
	}
}


void GroupTreeView::OnTextureSave()
{
	PatternPtr pattern = GetCurrentPattern();
	if (pattern)
	{
		Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_SAVE);

		std::string dir = Glib::get_current_dir();
 
		Gtk::FileFilter filter;
		filter.add_pattern("*.tx");

		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_CANCEL);
		std::string file = pattern->GetXMLFile();
		if (file=="") file = "new.tx";
		dialog.set_current_name(file);
		dialog.add_filter(filter);

		if (Gtk::RESPONSE_OK == dialog.run())
			pattern->SaveToFile(dialog.get_filename());
	}
}


void GroupTreeView::OnPatternLoad()
{
	PatternPtr pattern = GetCurrentPattern();
	GroupPtr parent = GetParentGroup(pattern);
	if (pattern && parent)
	{
		Gtk::FileChooserDialog dialog("Please select a file...", Gtk::FILE_CHOOSER_ACTION_SAVE);

		std::string dir = Glib::get_current_dir();
 
		Gtk::FileFilter filter;
		filter.add_pattern("*.pattern");
		filter.add_pattern("*.tx");

		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_CANCEL);
		std::string file = pattern->GetXMLFile();
		if (file=="") file = "new.pattern";
		dialog.set_current_name(file);
		dialog.add_filter(filter);

		if (Gtk::RESPONSE_OK == dialog.run())
		{
			PatternPtr newPattern = Pattern::Create(pattern->Dev(),dialog.get_filename()); 
			parent->ReplacePattern(pattern,newPattern);
			Refresh();
			Select(newPattern);
		}
	}
}


class GroupPanel : public Gtk::HBox
{
public:
	GroupPanel(GroupPtr peer) : peer_(peer)
	{
		Gtk::Box *box = manage(new Gtk::VBox);
		GroupOpCombo *opCombo = manage(new GroupOpCombo(peer));

		box->pack_start(*opCombo, Gtk::PACK_SHRINK);
		box->pack_start(opWidgetHolder_);
		OnOpChanged();
		pack_start(*box);
		pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);

		opCombo->SignalChanged().connect(
			sigc::mem_fun(*this, &GroupPanel::OnOpChanged));


		show_all_children();
	}

	void OnOpChanged()
	{
		// clear op widget
		Widget *old = opWidgetHolder_.get_child();
		opWidgetHolder_.remove();
		delete old;

		if (peer_->GetOperation() == Group::op_reactive)
		{
			ModulatorPtr mod = peer_->GetReactiveMod();
			if (mod)
			{
				Gtk::Box *box = manage(new Gtk::HBox);
				opWidgetHolder_.add(*box);

				ModulatorCombo *modCombo = manage(new ModulatorCombo(mod));
				modWidgetHolder_.set_shadow_type(Gtk::SHADOW_NONE);
				box->pack_start(*modCombo, Gtk::PACK_SHRINK);
				box->pack_start(modWidgetHolder_);

				Widget *old = modWidgetHolder_.get_child();
				modWidgetHolder_.remove();
				delete old;
				modWidgetHolder_.add(*manage(mod->CreateWidget(mod)));

				modCombo->SignalModulatorChanged().connect(
					sigc::mem_fun(*this, &GroupPanel::OnModulatorChanged));
			}
		}

		show_all_children();
	}

	void OnModulatorChanged(ModulatorPtr mod)
	{
		peer_->SetReactiveMod(mod);

		Widget *old = modWidgetHolder_.get_child();
		modWidgetHolder_.remove();
		delete old;

		modWidgetHolder_.add(*manage(mod->CreateWidget(mod)));
		show_all_children();
	}

	virtual ~GroupPanel() {}

protected:
	Gtk::Frame opWidgetHolder_;
	Gtk::Frame modWidgetHolder_;
	GroupPtr peer_;
};


class ComboTexturePanel : public Gtk::HBox
{
public:
	ComboTexturePanel(ComboTexturePtr peer, GroupTreeView *treeView, bool showSelector) : 
        treeView_(treeView), txCtrl_(peer), peer_(peer)
	{
		Gtk::VBox *lbox = manage(new Gtk::VBox);
		lbox->pack_start(*manage(new TextureInvertCtrl(peer)), Gtk::PACK_SHRINK);
		lbox->pack_start(*manage(new TextureAmplitudeCtrl(peer)));

		Gtk::Box *box = manage(new Gtk::VBox);
		box->pack_start(*manage(new GroupOpCombo(peer)), Gtk::PACK_SHRINK);
		box->pack_start(*manage(new OscillatorWidget(peer->GetOscillator(),true)), Gtk::PACK_SHRINK);

		if (showSelector)
		{
			pack_start(txCtrl_, Gtk::PACK_SHRINK);
			txCtrl_.SignalTextureChanged().connect(sigc::mem_fun(*this, &ComboTexturePanel::OnTextureChange));	
		}

		pack_start(*lbox, Gtk::PACK_SHRINK);
		pack_start(*box);
		pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);
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
	peer_(peer), treeView_(peer), txWidget_(NULL)
{
	Gtk::VBox *pSideBar = manage(new Gtk::VBox);
	Gtk::ScrolledWindow *scrolledTreeView = manage(new Gtk::ScrolledWindow);
	scrolledTreeView->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolledTreeView->set_placement(Gtk::CORNER_TOP_RIGHT);
	scrolledTreeView->add(treeView_);

	//pSideBar->pack_start(*manage(new GroupOpCombo(peer)), Gtk::PACK_SHRINK);
	pSideBar->pack_start(*scrolledTreeView);

	objWidgetHolder_.set_shadow_type(Gtk::SHADOW_NONE);
	pack_start(*pSideBar, Gtk::PACK_SHRINK);
	pack_start(objWidgetHolder_);

	treeView_.get_selection()->signal_changed().connect(
    		sigc::mem_fun(*this, &GroupWidget::OnSelectionChanged));

	// select the first item
	Gtk::TreeModel::Row row = treeView_.get_model()->children()[0];
	if (row) treeView_.get_selection()->select(row);
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
	Widget *old = objWidgetHolder_.get_child();
	objWidgetHolder_.remove();
	delete old;

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
				txWidget_ = manage(new TextureSelectorWidget(tx));
				currentTx_ = tx;
				objWidgetHolder_.add(*txWidget_);
				txWidget_->SignalTextureChanged().connect(sigc::mem_fun(*this, &GroupWidget::OnTextureChange));		
			}
			else if (combotx)
			{
				objWidgetHolder_.add(*manage(new ComboTexturePanel(combotx,&treeView_,hasParent)));
			}
			else if (dots)
			{
				objWidgetHolder_.add(*manage(dots->CreateWidget(dots)));
			}
			else
			{
				objWidgetHolder_.add(*manage(new GroupPanel(group)));
			}
		}
		else if (tx)
		{
			txWidget_ = manage(new TextureSelectorWidget(tx));
			currentTx_ = tx;
			objWidgetHolder_.add(*txWidget_);
			txWidget_->SignalTextureChanged().connect(sigc::mem_fun(*this, &GroupWidget::OnTextureChange));		
		}
		else
		{
			objWidgetHolder_.add(*manage(pattern->CreateWidget(pattern)));
		}
	}

	show_all_children();
}

} // namespace graphics
} // namespace latero

