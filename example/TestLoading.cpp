#include "TestLoading.h"

void TestLoading::ScreenShow()
{
	//A very noticeable Fuchsia
	_rectColor = GetColor(0xff, 0x00, 0xff);
	_lastProgress = 0;

	printf("Executing test loading. Please wait...\n");
}

void TestLoading::Update()
{
	//Checking and processing loading status
	SGB_LoadingScreenStatus stats;

	//Using a while() to process all queued status messages,
	//using if() works however it may cause a slow down on the
	//status process if there is more than one new status per
	//loop cycle cycle.
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
}

bool TestLoading::CheckLoading(bool loadingFinished)
{
	//we dont need user interaction, so proceed ASAP
	return loadingFinished;
}

void TestLoading::Draw()
{
	//Getting updated renderer dimensions
	int curWidth, curHeight;
	GetRendererSize(&curWidth, &curHeight);

	//The loading rect size will be:
	//horizontal: 90% width + 5% margin on each side
	//vertical: 10% height and 5% bottom margin
	SetRect(&_rect,
		(int)(curWidth * 0.05),
		(int)(curHeight - _rect.h - (curHeight * 0.05)),
		(int)((curWidth * 0.90) * _lastProgress),
		(int)(curHeight * 0.10)
		);

	SetColor(_rectColor);

	FillRect(_rect);
}
