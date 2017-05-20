#include "SGB_DisplayTimingManager.h"

#include <numeric>

SGB_DisplayTimingManager::SGB_DisplayTimingManager()
{
}

SGB_DisplayTimingManager::~SGB_DisplayTimingManager()
{
}

void SGB_DisplayTimingManager::Setup(SGB_DisplayInitInfo info)
{
	_initInfo = info;
	frameInterval = (1000 / _initInfo.TargetFrameRate);
	countedFrames = 0;
	lastCountReset = 0;
}

Uint32 SGB_DisplayTimingManager::StartGlobalTimer()
{
	fpsTimer.start();
	return fpsTimer.getTicks();
}

Uint32 SGB_DisplayTimingManager::CalculateAverageFPS()
{
	auto currentTicks = GetGlobalTicks();
	//Start cap timer
	capTimer.start();

	//Calculate and correct fps
	Uint32 split = (1000 / _initInfo.FrameRateSamplesPerSecond);
	Uint32 timeDiff = currentTicks - lastCountReset;

	if (timeDiff > split)
	{
		lastCountReset = currentTicks - (timeDiff - split);

		_fpsQueue.insert(
			_fpsQueue.end(),
			countedFrames);

		if (_fpsQueue.size() > _initInfo.FrameRateSamplesPerSecond)
		{
			_fpsQueue.erase(
				_fpsQueue.begin());
		}

		_currentAverageFPS = std::accumulate(
			_fpsQueue.begin(),
			_fpsQueue.end(),
			(Uint32)0);

		countedFrames = 0;
	}

	return _currentAverageFPS;
}

Uint32 SGB_DisplayTimingManager::GetGlobalTicks()
{
	return fpsTimer.getTicks();
}

Uint32 SGB_DisplayTimingManager::GetDeltaTicks()
{
	return stepTimer.getTicks();
}

void SGB_DisplayTimingManager::ResetDeltaTimer()
{
	stepTimer.start();
}

void SGB_DisplayTimingManager::FinishCycle()
{
	++countedFrames;

	//If frame finished early
	auto frameTicks = capTimer.getTicks();
	if (!_initInfo.UnlockFrameRate && frameTicks < frameInterval)
	{
		//Wait remaining time
		SDL_Delay(frameInterval - frameTicks);
	}
}
