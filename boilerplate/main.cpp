#include <iostream>

#include "MainDisplay.h"
#include "MainScreen.h"

int main(int argc, char** argv)
{
	MainDisplay myDisplay;

	if (myDisplay.Init() != SGB_SUCCESS)
	{
		std::cout << "Failed to init: " << SDL_GetError() << std::endl;
		return 1;
	}

	myDisplay.SetScreen(new MainScreen());

	while (myDisplay.IsRunning())
	{
		myDisplay.Update();
	}

	return 0;
}
