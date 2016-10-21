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

#include "oscillatorwidget.h"
#include "oscillator.h"
#include "gtk/numwidget.h"

namespace latero {
namespace graphics { 

class OscillatorBlendModeCombo : public Gtk::ComboBoxText
{
public:
	OscillatorBlendModeCombo(OscillatorPtr peer) : peer_(peer)
	{
		Oscillator::BlendModeSet ops = peer->GetBlendModes();
		for (unsigned int i=0; i<ops.size(); ++i)
			append_text(ops[i].label);
		set_active_text(peer->GetBlendMode().label);
		signal_changed().connect(sigc::mem_fun(*this, &OscillatorBlendModeCombo::OnChange));
	};
	virtual ~OscillatorBlendModeCombo() {};
private:
	void OnChange() { peer_->SetBlendMode(get_active_text()); };
	OscillatorPtr peer_;
};


OscillatorEnableCheck::OscillatorEnableCheck(OscillatorPtr peer) :
	Gtk::CheckButton("vibrate"), peer_(peer)
{
	set_active(peer->GetEnable());
}
void OscillatorEnableCheck::on_clicked() { peer_->SetEnable(!get_active()); Gtk::CheckButton::on_clicked(); };


OscillatorAmplitudeCtrl::OscillatorAmplitudeCtrl(OscillatorPtr peer) :
    adj_(peer->GetAmplitude()*100,0,100), peer_(peer)
{
	pack_start(*manage(new gtk::HNumWidget(adj_,0, units::percent)));
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &OscillatorAmplitudeCtrl::OnChanged));
}
void OscillatorAmplitudeCtrl::OnChanged() { peer_->SetAmplitude(adj_.get_value()/100); };


OscillatorFreqCtrl::OscillatorFreqCtrl(OscillatorPtr peer) :
    adj_(peer->GetFreq(),0.1,50), peer_(peer)
{
	pack_start(*manage(new gtk::HNumWidget(adj_,0, units::hz)));
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &OscillatorFreqCtrl::OnChanged));
}
void OscillatorFreqCtrl::OnChanged() { peer_->SetFreq(adj_.get_value()); };


OscillatorWidget::OscillatorWidget(OscillatorPtr peer, bool showBlendBode) :
	gtk::CheckFrame(peer->GetEnable(), "vibration"), peer_(peer)
{
	GetBox().pack_start(*manage(new OscillatorAmplitudeCtrl(peer)));
	GetBox().pack_start(*manage(new OscillatorFreqCtrl(peer)));
	if (showBlendBode) GetBox().pack_start(*manage(new OscillatorBlendModeCombo(peer)),Gtk::PACK_SHRINK);
	GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &OscillatorWidget::OnClick));
}
void OscillatorWidget::OnClick() { peer_->SetEnable(GetCheck().get_active()); }

} // namespace graphics
} // namespace latero
