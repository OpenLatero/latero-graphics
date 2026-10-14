#include <iostream>
#include <chrono>
#include <thread>
#include "vibrationgenerator.h"
#include <laterographics/tactileengine.h>

int main(int argc, char *argv[])
{
	// Create the Latero device object that will be used to communicate with the Latero.
	// We use a latero::Tactograph (latero::TactileDisplay with a position/orientation in
	// 2D space) because this is the class that the TactileEngine expects. We will however
	//use dev as if it was a latero::TactileDisplay (latero::TactileDisplay's base class). 
	latero::Tactograph dev;

	// Create a smart pointer to our custom latero::graphics::Generator that will render
	// frames for the Latero based on time.
	VibrationGeneratorPtr gen = VibrationGenerator::Create(&dev);

	// Create and start the tactile engine. The tactile engine runs in its own thread
	// (once Start() is called) and periodically calls the generator to render frames for the Latero.
	std::cout << "Starting tactile engine...\n";
	latero::graphics::TactileEngine tEngine(&dev, 750.0);
	tEngine.SetGenerator(gen);
	tEngine.Start();

	// While the tactile engine is running in its own thread, we periodically change the vibration
	// frequency by calling gen->SetFrequency(). We could also switch to a different generator
	// using tEngine.SetGenerator(). 
	double frequency = 1.0;
	while (frequency <= 128.0)
	{
		std::cout << "Frequency: " << frequency << " Hz\n";
		gen->SetFrequency(frequency);
		std::this_thread::sleep_for(std::chrono::seconds(10));
		frequency *= 2;
	}

	// When we're done, we call tEngine.Stop() which signals for the thread to end and waits
	// for completion.
	std::cout << "Stopping tactile engine...\n";
	tEngine.Stop();

	return 0;
}
