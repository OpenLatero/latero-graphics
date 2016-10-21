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

#include "modulatorcombo.h"
#include "modulator.h"
#include "dirmodulator.h"
#include "velmodulator.h"
#include <gtkmm/box.h>

namespace latero {
namespace graphics { 

void ModulatorCombo::Append(ModulatorPtr mod)
{
	Gtk::TreeModel::Row row = *(model_->append());
	row[columns_.mod] = mod;

	Glib::RefPtr<Gdk::Pixbuf> img  = mod->GetIllustration(50,50);
	if ((img->get_width() != 50) || (img->get_height() != 50))
		img = img->scale_simple(50,50,Gdk::INTERP_BILINEAR);
	row[columns_.img] = img;
}

void ModulatorCombo::SetActive(ModulatorPtr mod)
{
	if (!mod) return;

	// check if the modulator is already in the tree
	//bool found = false;
	for (Gtk::TreeModel::Children::iterator iter = model_->children().begin(); iter != model_->children().end(); iter++)
	{
		ModulatorPtr curmod = (*iter)[columns_.mod];
		if (curmod == mod)
		{
			signalEnable_ = false;
			set_active(iter); //  don't reload!
			signalEnable_ = true;
			return;
		}
	}

	Append(mod);
	SetActive(mod);
}


void ModulatorCombo::OnComboChanged()
{
	Gtk::TreeModel::iterator iter = get_active();
	if (!iter) return;

	Gtk::TreeModel::Row row = *iter;
	if (!row) return;

	if (signalEnable_)
		signalModulatorChanged_(row[columns_.mod]);
}

ModulatorCombo::ModulatorCombo(ModulatorPtr mod) :
	dev_(mod->Dev()),
	signalEnable_(true)
{
	set_wrap_width(1);

	model_ = Gtk::ListStore::create(columns_);
	set_model(model_);
	pack_start(columns_.img);

	Append(DirModulator::Create(dev_));
	DirModulatorPtr symDirmod = DirModulator::Create(dev_);
	symDirmod->SetSymmetric(true);
	Append(symDirmod);
	Append(VelModulator::Create(dev_));

	SetActive(mod);

	signal_changed().connect( sigc::mem_fun(*this, &ModulatorCombo::OnComboChanged) );
}

} // namespace graphics
} // namespace latero

