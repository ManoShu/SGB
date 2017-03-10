#include "TestScreen.h"

void TestScreen::LoadScreen()
{
	//Getting states
	keyStates = SDL_GetKeyboardState(NULL);

	//color for the boxes
	_boxColor = GetColor(0xff, 0x40, 0x40);

	SGB_LoadingScreenStatus loadingStatus;

	for (int i = 0; i < 100; i++)
	{
		loadingStatus.progress = (float)(i / 100.0);
		sprintf(loadingStatus.status, "Loading %d %%...", i);
		PushLoadingStatus(loadingStatus);
		SDL_Delay(50);
	}

	loadingStatus.progress = 1;
	sprintf(loadingStatus.status, "Loading 100 %%... Done.");
	PushLoadingStatus(loadingStatus);
}

void TestScreen::ScreenShow()
{
	_elapsedCounter = 0;
	printf("Press ESC to finish.\n");
}

void TestScreen::Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	SDL_PumpEvents();

	if (keyStates[SDL_SCANCODE_ESCAPE])
	{
		*isRunning = false;
	}

	//Getting updated renderer dimensions
	int curWidth, curHeight;
	GetRendererSize(&curWidth, &curHeight);

	//drawing fps boxes
	SDL_Rect rect = GetRect(
		0,
		0,
		(int)((curWidth / 10.0) - 5),
		(int)((curHeight * 0.10) - 5));

	SetColor(_boxColor);

	int i = 0, j = 0;

	Uint32 fpsCounter = 0;
	
	while (fpsCounter < avgFPS)
	{
		if (i > 9)
		{
			i = 0;
			j++;
		}

		SetRect(&rect,
			2 + (i * rect.w) + (i * 5),
			2 + (j * rect.h) + (j * 5),
			rect.w,
			rect.h);

		FillRect(rect);

		i++;
		fpsCounter++;
	}


	_elapsedCounter += deltaT;

	//after 10 seconds has passed, change to another screen
	if (_elapsedCounter >= 10)
	{
		SetNextScreen(new TestScreen());
	}
}


