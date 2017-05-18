#include <cstdio>

#include "TestDisplay.h"
#include "TestScreen.h"
#include "TestLoading.h"

int main(int argc, char** argv)
{
	bool isRunning = true;

	TestDisplay myDisplay;

	if (myDisplay.Init() != SGB_SUCCESS)
	{
		printf("Failed to init: %s\n", SDL_GetError());
		return 1;
	}

	myDisplay.SetLoadingScreen(new TestLoading());
	myDisplay.SetScreen(new TestScreen());

	while (isRunning)
	{
		myDisplay.Update(&isRunning);
	}

	return 0;
}
