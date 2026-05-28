#include <filesystem>
#include "modulatorpreview.h"
#include "modulator.h"
#include <glibmm/main.h>
#include <giomm/simpleactiongroup.h>

namespace latero::graphics {

ModulatorPreview::ModulatorPreview(ModulatorPtr peer) : peer_(peer)
{
	append(img_);
	Refresh();
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &ModulatorPreview::OnTimer),
		(uint)333, // ms
		Glib::PRIORITY_DEFAULT_IDLE);
	CreatePopupMenu();
}

void ModulatorPreview::CreatePopupMenu()
{
	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &ModulatorPreview::OnClick));
	add_controller(gesture);

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("saveas", sigc::mem_fun(*this, &ModulatorPreview::OnSaveAs));
	action_group->add_action("save",   sigc::mem_fun(*this, &ModulatorPreview::OnSave));
	insert_action_group("modulator", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  	<menu id="PopupMenu">
    <item>
      <attribute name="label">save as</attribute>
      <attribute name="action">modulator.saveas</attribute>
    </item>
    <item>
      <attribute name="label">save</attribute>
      <attribute name="action">modulator.save</attribute>
    </item>
  	</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu_model);
	popupMenu_->set_parent(*this);
}

void ModulatorPreview::OnSaveAs()
{
	auto dialog = Gtk::FileDialog::create();
	dialog->set_title("Please select file name.");
	dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
	dialog->set_initial_name("modulation.png");
	auto* win = dynamic_cast<Gtk::Window*>(get_root());
	dialog->save(*win, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto file = dialog->save_finish(result);
			peer_->GetIllustration(1000,50)->save(file->get_path(),"png");
		} catch (const Gtk::DialogError&) {}
	});
}

void ModulatorPreview::OnSave()
{
	printf("saving modulator.png\n");
	peer_->GetIllustration(1000,50)->save("modulator.png","png");
}

	
void ModulatorPreview::OnClick(int n_press, double x, double y)
{
	popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
	popupMenu_->popup();
}

bool ModulatorPreview::OnTimer()
{
	if (peer_->GetLastModified() > refreshTime_)
		Refresh();
	return true;
}

void ModulatorPreview::Refresh()
{
	img_.set(peer_->GetIllustration(200,50));
	refreshTime_ = boost::posix_time::microsec_clock::universal_time();
}

} // namespace
