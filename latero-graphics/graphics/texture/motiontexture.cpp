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

#include "motiontexture.h"
#include "motiontexturewidget.h"
#include "lineargratingtexture.h"
#include "texture.h"
#include "stock.h"
#include "../../grating.h"
#include "../../xml.h"
#include <iostream>

namespace latero {
namespace graphics { 

const MotionTexture::CueType MotionTexture::cue_type_none(0,"none");
const MotionTexture::CueType MotionTexture::cue_type_vib_bands(1,"vibration bands");
const MotionTexture::CueType MotionTexture::cue_type_superposed_bands(2,"superposed bands");
const MotionTexture::CueType MotionTexture::cue_type_superposed_vib_bands(3,"superposed vibration bands");
const MotionTexture::CueType MotionTexture::cue_type_disabling_bands(4,"disabling bands");
const MotionTexture::CueType MotionTexture::cue_type_blend_bands(5,"blending bands");

MotionTexture::MotionTexture(const latero::Tactograph *dev) :
	Pattern(dev), ComboTexture(dev), cueType_(cue_type_none)
{
	SetDefaults();
}

MotionTexture::MotionTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), ComboTexture(dev), cueType_(cue_type_none)
{
	SetDefaults();
	LoadXML(node);
}

void MotionTexture::SetDefaults()
{
	cueType_ = cue_type_none;
	vibTx_ = VibroTexture::Create(Dev());
	motionTx_ = LinearGratingTexture::Create(Dev());
	plainTx_ = PlainTexture::Create(Dev());

	cueTypeSet_.Append(cue_type_none);
	cueTypeSet_.Append(cue_type_vib_bands);
	cueTypeSet_.Append(cue_type_superposed_bands);
	cueTypeSet_.Append(cue_type_superposed_vib_bands);
	cueTypeSet_.Append(cue_type_disabling_bands);
	cueTypeSet_.Append(cue_type_blend_bands);

	SetVelocity(velocity_default);
	SetDirection(direction_default);

	tx_ = LinearGratingTexture::Create(Dev(),8,0);
	ClearPatterns();
	InsertPattern(tx_);
}

Gtk::Widget *MotionTexture::CreateWidget(TexturePtr ptr)
{
	MotionTexturePtr gen = boost::dynamic_pointer_cast<MotionTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new MotionTextureWidget(gen);

}

void MotionTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Motion");
	ComboTexture::AppendIcon(node);

	node.AddChild("direction", GetDirection(), units::degree);
	node.AddChild("velocity", GetVelocity(), units::mm_per_sec);
	node.AddChild("cue_type", GetCueType().label);
	tx_->AppendXML(node);
}

void MotionTexture::LoadXML(const XMLInputNode &root)
{
	ComboTexture::LoadIcon(root);

	XMLInputNode node = root.GetChild("direction");
	if (node) SetDirection(node.GetFloat());

	node = root.GetChild("velocity");
	if (node) SetVelocity(node.GetFloat());

	node = root.GetChild("cue_type");
	if (node) SetCueType(node.GetString());

	node = root.GetChild("Pattern");
	if (node) SetTexture(Texture::Create(Dev(),node));
}

std::string MotionTexture::GetDefaultIconFile() const
{
	return tx_icon_motion; // todo
}

void MotionTexture::SetCueType(std::string label)
{
	CueType *op = cueTypeSet_.Get(label);
	if (op) SetCueType(*op);
}

void MotionTexture::SetTexture(TexturePtr tx)
{
	LATERO_GRAPHICS_GUARD;
	ComboTexture::ReplacePattern(0, tx);
	tx_=tx;
}

void MotionTexture::SetCueType(const CueType &t)
{
	LATERO_GRAPHICS_GUARD;
	cueType_ = t;
	SetLastModified_();

	ClearPatterns();
	if (cueType_ == cue_type_none)
	{
		ClearPatterns();
		InsertPattern(tx_);
		tx_->SetAmplitude(1.0);
	}
	else if (cueType_ == cue_type_vib_bands)
	{
		InsertPattern(tx_);
		InsertPattern(vibTx_);
		InsertPattern(motionTx_);
		tx_->SetAmplitude(1.0);
		motionTx_->SetAmplitude(1.0);
		motionTx_->GetGrating()->SetRidgeSize(5);
		motionTx_->GetGrating()->SetGapSize(30);		
		motionTx_->SetInvert(true);
		motionTx_->SetTDCentric(true);
		motionTx_->SetVibAmplitude(0);
		SetOperation(ComboTexture::op_blend);
	}
	else if (cueType_ == cue_type_superposed_bands)
	{
		InsertPattern(tx_);
		InsertPattern(motionTx_);
		tx_->SetAmplitude(0.8);
		motionTx_->SetAmplitude(0.2);
		motionTx_->GetGrating()->SetRidgeSize(2);
		motionTx_->GetGrating()->SetGapSize(30);		
		motionTx_->SetInvert(false);
		motionTx_->SetTDCentric(true);
		motionTx_->SetVibAmplitude(0);
		SetOperation(ComboTexture::op_add);
	}
	else if (cueType_ == cue_type_superposed_vib_bands)
	{
		InsertPattern(tx_);
		InsertPattern(motionTx_);
		tx_->SetAmplitude(0.8);
		motionTx_->SetAmplitude(0.2);
		motionTx_->GetGrating()->SetRidgeSize(5);
		motionTx_->GetGrating()->SetGapSize(30);		
		motionTx_->SetInvert(false);
		motionTx_->SetTDCentric(true);
		motionTx_->SetVibAmplitude(1.0);
		SetOperation(ComboTexture::op_add);
	}
	else if (cueType_ == cue_type_disabling_bands)
	{
		InsertPattern(tx_);
		InsertPattern(motionTx_);
		tx_->SetAmplitude(1.0);
		motionTx_->SetAmplitude(1.0);
		motionTx_->GetGrating()->SetRidgeSize(2);
		motionTx_->GetGrating()->SetGapSize(30);		
		motionTx_->SetInvert(true);
		motionTx_->SetTDCentric(true);
		motionTx_->SetVibAmplitude(0.0);
		SetOperation(ComboTexture::op_multiply);
	}
	else if (cueType_ == cue_type_blend_bands)
	{
		InsertPattern(tx_);
		InsertPattern(plainTx_);
		InsertPattern(motionTx_);
		plainTx_->SetAmplitude(0.5);
		tx_->SetAmplitude(1.0);
		motionTx_->SetAmplitude(1.0);
		motionTx_->GetGrating()->SetRidgeSize(2);
		motionTx_->GetGrating()->SetGapSize(30);		
		motionTx_->SetInvert(true);
		motionTx_->SetTDCentric(true);
		motionTx_->SetVibAmplitude(0);
		SetOperation(ComboTexture::op_blend);
	}
}

void MotionTexture::SetVelocity(double v)
{
	LATERO_GRAPHICS_GUARD;
	velocity_=v;
	SetLastModified_();
	motionTx_->GetGrating()->SetVelUnitsHz(false); // TODO!
	motionTx_->GetGrating()->SetVel(v);
}

void MotionTexture::SetDirection(double v)
{
	LATERO_GRAPHICS_GUARD;
	direction_=v;
	SetLastModified_();

	motionTx_->SetAngle(v);
}

} // namespace graphics
} // namespace latero
