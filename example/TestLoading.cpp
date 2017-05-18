#include "TestLoading.h"

void TestLoading::ScreenShow()
{
	//A very noticeable Fuchsia
	_rectColor = _display->GetColor(0xff, 0x00, 0xff, SDL_ALPHA_OPAQUE);
	_lastProgress = 0;

	printf("Executing dummy loading. Please wait...\n");
}

void TestLoading::Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	//Checking and processing loading status
	SGB_LoadingScreenStatus stats;

	//Using a while() to process all queued status messages,
	//using if() may cause slow down status process if there is
	//more than one new status per update cycle
	while (PullLoadingStatus(&stats) == SGB_SUCCESS)
	{		
		//at each PROGRESS_THRESHOLD% we output the progress on the console
		const int PROGRESS_THRESHOLD = 5;

		int iProgress = (int)(stats.progress * 100);
		int iLastProgress = (int)(_lastProgress * 100);

		if ((iProgress / PROGRESS_THRESHOLD) != (iLastProgress / PROGRESS_THRESHOLD))
		{
			printf("Progress: %.0f%%, Status: %s\n", stats.progress * 100, stats.status);
		}

		_lastProgress = stats.progress;
	}

	//Getting updated renderer dimensions
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	//The loading rect size will be:
	//horizontal: 90% width + 5% margin on each side
	//vertical: 10% height and 5% bottom margin
	_rect.x = (int)(curWidth * 0.05);
	_rect.h = (int)(curHeight * 0.10);
	_rect.y = (int)(curHeight - _rect.h - (curHeight * 0.05));
	_rect.w = (int)((curWidth * 0.9) * _lastProgress);

	_display->SetDrawColor(_rectColor);

	SDL_RenderFillRect(_renderer, &_rect);
}

bool TestLoading::CheckLoading(bool loadingFinished)
{
	if (loadingFinished)
	{
		//we dont need user interaction, so proceed ASAP
		return true;
	}

	return false;
}
