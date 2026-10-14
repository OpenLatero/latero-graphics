#include <iostream>
#include <gtkmm.h>
#include "mainwindow.h"
#include "vibrationgenerator.h"
#include <laterographics/tactileengine.h>

int main(int argc, char *argv[])
{
	latero::Tactograph dev;
	latero::graphics::GeneratorPtr gen = VibrationGenerator::Create(&dev);

	std::cout << "Starting tactile engine...\n";
	latero::graphics::TactileEngine tEngine(&dev, 750.0);
	tEngine.SetGenerator(gen);
	tEngine.Start();
	
	// Create and run MainWindow. This will block until the window is closed.
	std::cout << "Starting GUI...\n";
	auto app = Gtk::Application::create("org.openlatero.latero-example-vibrate-gui");
	int rv = app->make_window_and_run<MainWindow>(argc, argv, &dev, gen);

	std::cout << "Stopping tactile engine...\n";
	tEngine.Stop();

	return rv;
}
