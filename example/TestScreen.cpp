#include "TestScreen.h"

void TestScreen::LoadScreen()
{
	//Getting states
	keyStates = SDL_GetKeyboardState(NULL);

	//color for the boxes
	_boxColor = GetColor(0xff, 0x40, 0x40);

	SGB_LoadingScreenStatus loadingStatus;

	const int BAR_ITERATIONS = 1000;
	const float BAR_ITERATIONS_F = BAR_ITERATIONS * 1.0f;
	// the bar will scroll in 5 seconds
	const int ITERATION_DELAY = 5000 / BAR_ITERATIONS;

	for (int i = 0; i < BAR_ITERATIONS; i++)
	{
		loadingStatus.progress = (float)(i / BAR_ITERATIONS_F);
		snprintf(loadingStatus.status, SGB_LOADING_MESSAGE_SIZE, "Dummy loading %d of %d...", i, BAR_ITERATIONS);
		PushLoadingStatus(loadingStatus);
		SDL_Delay(ITERATION_DELAY);
	}

	loadingStatus.progress = 1;
	snprintf(loadingStatus.status, SGB_LOADING_MESSAGE_SIZE, "Dummy loading done.");
	PushLoadingStatus(loadingStatus);
}

void TestScreen::ScreenShow()
{
	_elapsedCounter = 0;
	printf("Showing FPS as blocks. Press ESC to finish.\n");
}

void TestScreen::Update()
{
	SDL_PumpEvents();

	if (keyStates[SDL_SCANCODE_ESCAPE])
	{
		_display->StopRunning();
	}

	auto stats = _display->GetLoopStats();

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

	while (fpsCounter < stats.AverageFrameRate)
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


	_elapsedCounter += stats.DeltaSeconds;

	//after 10 seconds has passed, change to another screen
	if (_elapsedCounter >= 10)
	{
		SetNextScreen(new TestScreen());
	}
}


