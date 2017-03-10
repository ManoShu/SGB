#include "TestLoading.h"


void TestLoading::ScreenShow()
{
	//A very noticeable Fuchsia
	_rectColor = _display->GetColor(0xff, 0x00, 0xff, SDL_ALPHA_OPAQUE);
	_lastProgress = 0;
	
	printf("Loading. Please wait...\n");
}

void TestLoading::Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	//Checking and processing loading status
	SGB_LoadingScreenStatus stats;
	if (PullLoadingStatus(&stats) == SGB_SUCCESS)
	{
		if(((int)(stats.progress * 100)) % 10 == 0)
		{ 
			printf("Progress: %f, Status: %s\n", stats.progress, stats.status);
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
		return true;
	}

	return false;
}
