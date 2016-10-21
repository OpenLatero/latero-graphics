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

#include "doublelineargratingtexture.h"
#include "doublelineargratingtexturewidget.h"
#include "lineargratingtexture.h"
#include "texture.h"
#include "stock.h"
#include "../../grating.h"
#include "../../xml.h"

namespace latero {
namespace graphics { 

DoubleLinearGratingTexture::DoubleLinearGratingTexture(const latero::Tactograph *dev) : 
	Pattern(dev), ComboTexture(dev),
	constraint_(constraint_default), secScale_(1)
{
	Texture::motionOverride_ = true;
	tx_[0] = LinearGratingTexture::Create(dev,8,0);
	tx_[1] = LinearGratingTexture::Create(dev,8,90);
	InsertPattern(tx_[0]);
	InsertPattern(tx_[1]);
}

DoubleLinearGratingTexture::DoubleLinearGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), ComboTexture(dev),
	constraint_(constraint_default), secScale_(1)
{
	Texture::motionOverride_ = true;
	tx_[0] = LinearGratingTexture::Create(dev,8,0);
	tx_[1] = LinearGratingTexture::Create(dev,8,90);
	InsertPattern(tx_[0]);
	InsertPattern(tx_[1]);
	LoadXML(node);
}

Gtk::Widget *DoubleLinearGratingTexture::CreateWidget(TexturePtr ptr)
{
	DoubleLinearGratingTexturePtr gen = boost::dynamic_pointer_cast<DoubleLinearGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new DoubleLinearGratingTextureWidget(gen);
}

Gtk::Widget *DoubleLinearGratingTexture::CreateAdvancedWidget(TexturePtr ptr)
{
	DoubleLinearGratingTexturePtr gen = boost::dynamic_pointer_cast<DoubleLinearGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return new DoubleLinearGratingTextureAdvancedWidget(gen);
}



void DoubleLinearGratingTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("DoubleLinearGrating");

	node.AddChild("seed", GetSeed(), units::mm);
	node.AddChild("angle", GetAngle(), units::degree);
	node.AddChild("primary_ridge_size", GetPrimaryRidgeSize(), units::mm);
	node.AddChild("primary_gap_size", GetPrimaryGapSize(), units::mm);
	node.AddChild("secondary_scale", GetSecondaryScale());
	if (GetConstraint() == constraint_ortho)
		node.AddChild("constraint", "orthogonal");
	else
		node.AddChild("constraint", "parallel");
	node.AddChild("primary_amplitude", GetPrimaryAmplitude());
	node.AddChild("secondary_amplitude", GetSecondaryAmplitude());
	AppendOperation(node);
	AppendIcon(node);
	AppendMotion(root);
}

void DoubleLinearGratingTexture::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("seed");
	if (node) SetSeed(node.GetPoint());

	node = root.GetChild("angle");
	if (node) SetAngle(node.GetFloat());

	node = root.GetChild("primary_ridge_size");
	if (node) SetPrimaryRidgeSize(node.GetFloat());

	node = root.GetChild("primary_gap_size");
	if (node) SetPrimaryGapSize(node.GetFloat());

	node = root.GetChild("secondary_scale");
	if (node) SetSecondaryScale(node.GetFloat());

	node = root.GetChild("constraint");
	if (node)
	{
		if (node.GetString() == "orthogonal")
	 		SetConstraint(constraint_ortho);
		else
			SetConstraint(constraint_parallel);
	}

	node = root.GetChild("primary_amplitude");
	if (node) SetPrimaryAmplitude(node.GetFloat());

	node = root.GetChild("secondary_amplitude");
	if (node) SetSecondaryAmplitude(node.GetFloat());

	LoadOperation(root);
	LoadIcon(root);
	LoadMotion(root);
}

std::string DoubleLinearGratingTexture::GetDefaultIconFile() const
{
	return tx_icon_double_linear_grating;
}

void DoubleLinearGratingTexture::SetAngle(double v)
{
	LATERO_GRAPHICS_GUARD;
	tx_[0]->SetAngle(v);
	tx_[1]->SetAngle((constraint_ == constraint_ortho)?v+90:v);
	UpdateChildMotion();
}

double DoubleLinearGratingTexture::GetAngle() const
{
	LATERO_GRAPHICS_GUARD;
	return tx_[0]->GetAngle();
}

void DoubleLinearGratingTexture::SetSeed(const Point &p)
{
	LATERO_GRAPHICS_GUARD;
	tx_[0]->SetSeed(p);
	tx_[1]->SetSeed(p);
}

Point DoubleLinearGratingTexture::GetSeed() const
{
	LATERO_GRAPHICS_GUARD;
	return tx_[0]->GetSeed();
}

void DoubleLinearGratingTexture::SetPrimaryRidgeSize(double v) 
{
	LATERO_GRAPHICS_GUARD;
	tx_[0]->GetGrating()->SetRidgeSize(v);
	tx_[1]->GetGrating()->SetRidgeSize(v*secScale_);
}

double DoubleLinearGratingTexture::GetPrimaryRidgeSize() const
{ LATERO_GRAPHICS_GUARD; return tx_[0]->GetGrating()->GetRidgeSize(); }

void DoubleLinearGratingTexture::SetSecondaryScale(double v)
{
	LATERO_GRAPHICS_GUARD;
	secScale_ = v;
	tx_[1]->GetGrating()->SetRidgeSize(v * tx_[0]->GetGrating()->GetRidgeSize());
	tx_[1]->GetGrating()->SetGapSize(v * tx_[0]->GetGrating()->GetGapSize());
}

double DoubleLinearGratingTexture::GetSecondaryScale() const
{ LATERO_GRAPHICS_GUARD; return secScale_; }

void DoubleLinearGratingTexture::SetPrimaryGapSize(double v) 
{
	LATERO_GRAPHICS_GUARD;
	tx_[0]->GetGrating()->SetGapSize(v);
	tx_[1]->GetGrating()->SetGapSize(v*secScale_);
}

double DoubleLinearGratingTexture::GetPrimaryGapSize() const
{ LATERO_GRAPHICS_GUARD; return tx_[0]->GetGrating()->GetGapSize(); }

void DoubleLinearGratingTexture::SetConstraint(int mode)
{ LATERO_GRAPHICS_GUARD; constraint_=mode; SetAngle(GetAngle()); }

int DoubleLinearGratingTexture::GetConstraint() const
{ LATERO_GRAPHICS_GUARD; return constraint_; }

void DoubleLinearGratingTexture::SetPrimaryAmplitude(double v)
{ LATERO_GRAPHICS_GUARD; tx_[0]->SetAmplitude(v); }

double DoubleLinearGratingTexture::GetPrimaryAmplitude() const
{ LATERO_GRAPHICS_GUARD; return tx_[0]->GetAmplitude(); }

void DoubleLinearGratingTexture::SetSecondaryAmplitude(double v)
{ LATERO_GRAPHICS_GUARD; tx_[1]->SetAmplitude(v); }

double DoubleLinearGratingTexture::GetSecondaryAmplitude() const
{ LATERO_GRAPHICS_GUARD; return tx_[1]->GetAmplitude(); }

void DoubleLinearGratingTexture::UpdateChildMotion()
{
	for (int i=0; i<2; ++i)
	{
		if (!GetMotionEnable())
			tx_[i]->GetGrating()->SetVel(0);
		else
		{
			tx_[i]->GetGrating()->SetVelUnitsHz(false);
			double theta = units::DegreeToRad(GetMotionDirection() - tx_[i]->GetAngle());
			double v = GetMotionVelocity() * cos(theta);
			if (fabs(v) < 0.001) v = 0;
			tx_[i]->GetGrating()->SetVel(v);
		}
	}
}

} // namespace graphics
} // namespace latero
