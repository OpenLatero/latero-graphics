#pragma once

#include "texturefwd.h"
#include <gtkmm.h>
#include <latero/tactograph.h>

namespace latero::graphics {

class TextureDropDown : public Gtk::Box
{
public:
	TextureDropDown(TexturePtr tx, std::vector<std::string> textures = std::vector<std::string>());
	TextureDropDown(const latero::Tactograph *dev, std::vector<std::string> textures = std::vector<std::string>());

	virtual ~TextureDropDown() {};
	void Append(std::string txfile);
	void SetActive(TexturePtr tx);
	sigc::signal<void(TexturePtr)> SignalTextureChanged() { return signalTextureChanged_; }

	TexturePtr GetTexture() { return tx_; }
protected:
	sigc::signal<void(TexturePtr)> signalTextureChanged_;
	void Init();
	void OnDropDownChanged();

    bool signalEnable_;
	TexturePtr tx_;
	
	Glib::RefPtr<Gtk::StringList> countingList_;
	Gtk::DropDown dropDown_;

    const latero::Tactograph *dev_; // tmp?

	// TODO: merge those three items in a single struct/class
	std::vector<std::string> txFileList_;
	std::vector<std::string> imgFileList_;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufList_;
	std::vector<std::string> textureList_;
};

} // namespace

