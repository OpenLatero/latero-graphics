#include "imagewidget.h"
#include "image.h"

namespace latero::graphics {

ImageWidget::ImageWidget(ImagePtr peer, bool showMaskWidget) :
	Gtk::Box(Gtk::Orientation::VERTICAL),
	txWidget_(peer->GetTexture()),
	maskWidget_(peer->GetMask(),peer->Dev()),
	peer_(peer)
{
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	append(*box);
	box->set_vexpand();
	
	if (showMaskWidget)
	{
		Gtk::Notebook *notebook = Gtk::make_managed<Gtk::Notebook>();
		notebook->append_page(maskWidget_, "mask");
		notebook->append_page(txWidget_, "texture");
		box->append(*notebook);
		notebook->set_hexpand();
	}
	else
	{
		box->append(txWidget_);
		txWidget_.set_hexpand();
	}
	//box->append(vizButton_);


	txWidget_.SignalTextureChanged().connect(
		sigc::mem_fun(*this, &ImageWidget::OnTextureChanged));
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::OnTextureChanged()
{
	peer_->SetTexture(txWidget_.GetTexture());
}

} // namespace
