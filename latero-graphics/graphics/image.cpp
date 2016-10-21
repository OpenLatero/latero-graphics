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

#include "image.h"
#include "imagewidget.h"
#include "../gtk/pixbufops.h"
#include "../xml.h"
#include "texture/lineargratingtexture.h"
#include "texture/vibrotexture.h"
#include "texture/texture.h"
#include "../mask.h"
#include "texture/nulltexture.h"

namespace latero {
namespace graphics { 

Image::Image(const latero::Tactograph *dev, TexturePtr texture) :
	Pattern(dev),
	mask_(Mask::Create(dev->GetSurfaceSize())),
	enable_(true)
{
	if (!texture) texture = NullTexture::Create(dev);
	SetTexture(texture);
	AddModifiableChild(GetMask());
	mask_->SetBlurred(true);
};


Image::Image(const latero::Tactograph *dev, Glib::RefPtr<Gdk::Pixbuf> mask, TexturePtr texture) :
	Pattern(dev),
	mask_(Mask::Create(dev->GetSurfaceSize())),
	enable_(true)
{
	if (!texture) texture = NullTexture::Create(dev);
	AddModifiableChild(GetMask());
	mask_->SetBlurred(true);
	mask_->SetImage(mask,false);
	SetTexture(texture);
};


Image::Image(const latero::Tactograph *dev, std::string mask, TexturePtr texture) :
	Pattern(dev),
	mask_(Mask::Create(dev->GetSurfaceSize())),
	enable_(true)
{
	if (!texture) texture = NullTexture::Create(dev);
	AddModifiableChild(GetMask());
	mask_->SetBlurred(true);
	mask_->SetImage(mask,false);
	SetTexture(texture);
};

Image::Image(const latero::Tactograph *dev, const XMLInputNode &xmlNode) :
	Pattern(dev),
	mask_(Mask::Create(dev->GetSurfaceSize())),
	enable_(true)
{
	SetTexture(NullTexture::Create(dev));
	AddModifiableChild(GetMask());
	LoadXML(xmlNode);
};

void Image::LoadXML(const XMLInputNode &root)
{
	XMLInputNode textureNode = root.GetChild("Pattern");
	SetTexture(Texture::Create(Dev(),textureNode));
	mask_->LoadXML(root);
}

void Image::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Image");
	texture_->AppendXML(node);
	mask_->AppendXML(node);
}

Image::~Image()
{
	RemoveModifiableChild(mask_);
}

double Image::DoRender_(const ActuatorState &state)
{
	if (!enable_) return 0.0;
	float maskValue = mask_->Get_(state.pos);
	if (maskValue <= 0) return 0.0;
	return maskValue*texture_->Render_(state);
}

double Image::DoRenderShadow_(const ActuatorState &state)
{
	if (!enable_) return 0.0;
	return mask_->GetShadow_(state.pos);
}


latero::graphics::gtk::Animation Image::GetVibroIllustration(uint w, uint h)
{
	VibroTexturePtr vibroTx = boost::dynamic_pointer_cast<VibroTexture>(GetTexture());
	double stroke = vibroTx->GetAmplitude();

	Glib::RefPtr<Gdk::Pixbuf> buf = mask_->GetSurfaceAlpha(w, h);
	latero::graphics::gtk::Animation rv;
	for (int i=0; i<2; ++i)
	{
		Glib::RefPtr<Gdk::Pixbuf> frame = buf->copy(); // ~ 3 ms

		uint rowstride = frame->get_rowstride();
		uint n_channels = frame->get_n_channels();
		guint8* pixels = frame->get_pixels();

		for (unsigned int y=0; y<h; ++y)
		{
			for (unsigned int x=0; x<w; ++x)
			{
				guint8* pix = pixels + y*rowstride + x*n_channels;
				float noise = (rand() % 256) / 255.0;
				uint orig = pix[3];	
				pix[0] = pix[1] = pix[2] = (unsigned char)(255 - (stroke*noise*orig));
				pix[3] = 255;
			}
		}

		rv.AppendFrame(frame);
	}

	return rv;
}


Cairo::RefPtr<Cairo::Pattern> Image::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();
	DrawIllustration(mmContext,t);
	return mmContext->pop_group();
}

void Image::DrawIllustration(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	Point size = mask_->GetSize();
	Point offset = mask_->GetOffset();

	// clip outside of mask if possible
	if (mask_->GetDefaultAlpha() == 0)
	{
		mmContext->rectangle(offset.x, offset.y, size.x, size.y);
		mmContext->clip();
	}

	// set the grating pattern as a source
	Cairo::RefPtr<Cairo::Pattern> pattern = GetTexture()->GetDrawingPattern(mmContext,t);
	mmContext->set_source(pattern);

	// paint according to the masks's alpha channel
	Glib::RefPtr<Gdk::Pixbuf> alpha = mask_->GetSurfaceAlpha(2000,2000); // TODO: pick better value
	Cairo::RefPtr<Cairo::ImageSurface> alphaSurface = Cairo::ImageSurface::create(
		(unsigned char*)alpha->get_pixels(),
		Cairo::FORMAT_ARGB32,
		alpha->get_width(),
		alpha->get_height(),
		alpha->get_rowstride());
	mmContext->scale(Dev()->GetSurfaceWidth()/alphaSurface->get_width(), Dev()->GetSurfaceHeight()/alphaSurface->get_height());
	mmContext->mask(alphaSurface,0,0);
}

void Image::Enable(bool v)
{
	LATERO_GRAPHICS_GUARD
	SetLastModified_();
	enable_ = v;
}

Gtk::Widget *Image::CreateWidget(PatternPtr ptr)
{
	ImagePtr gen = boost::dynamic_pointer_cast<Image>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new ImageWidget(gen);
}

void Image::SetTexture(TexturePtr texture)
{
	LATERO_GRAPHICS_GUARD;
	if (texture_) RemoveModifiableChild(texture_);
	texture_ = texture;
	AddModifiableChild(texture_);
	SetLastModified_(); 
}

bool Image::UpdateVisible_(const State *state)
{
	if (mask_->GetDefaultAlpha_() == 0)
	{
		bool visible = false;
		Point p1 = mask_->GetOffset_();
		Point p2 = p1 + mask_->GetSize_();
		double r = Dev()->GetRadius();
		visible = (state->pos.x >= p1.x - r) &&
			  (state->pos.y >= p1.y - r) &&
		  	  (state->pos.x <= p2.x + r) &&
		  	  (state->pos.y <= p2.y + r);
		SetVisible_(visible);
		return visible;
	}
	else
	{
		SetVisible_(true);
		return true;
	}

}


} // namespace graphics
} // namespace latero

