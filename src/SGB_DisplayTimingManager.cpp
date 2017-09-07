#include "SGB_DisplayTimingManager.h"

#include <numeric>

void SGB_DisplayTimingManager::Setup(SGB_DisplayInitInfo info)
{
	_initInfo = info;
}

Uint32 SGB_DisplayTimingManager::StartGlobalTimer()
{
	_currentTime = SDL_GetTicks();
	_totalElapsed = 0;
	_loopElapsed = 0;
	_lastFPSReset = _currentTime;
	_currentAverageFPS = 0;
	
	_frameInterval = (1000 / _initInfo.TargetFrameRate);
	
	return _currentTime;
}

Uint32 SGB_DisplayTimingManager::CalculateAverageFPS()
{
	//Calculate and correct fps
	Uint32 split = (1000 / _initInfo.FrameRateSamplesPerSecond);
	
	auto timeDiff = _currentTime - _lastFPSReset;

	if (timeDiff > split)
	{
		auto spareTime = (timeDiff - split);
		
		_lastFPSReset = _currentTime - spareTime;

		_fpsQueue.insert(_fpsQueue.end(),countedFrames);

		if (_fpsQueue.size() > _initInfo.FrameRateSamplesPerSecond)
		{
			_fpsQueue.erase(_fpsQueue.begin());
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
	return _currentTime;
}

Uint32 SGB_DisplayTimingManager::GetDeltaTicks()
{
	return _loopElapsed;
}

void SGB_DisplayTimingManager::UpdateFrameData()
{
	auto theTime = SDL_GetTicks();
	_loopElapsed = theTime - _currentTime;
	_currentTime = theTime;
}

bool SGB_DisplayTimingManager::CanRender()
{
	auto result = false;
	
	_totalElapsed += _loopElapsed;

	if (_totalElapsed >= _frameInterval || _initInfo.UnlockFrameRate)
	{
		++countedFrames;
		
		result = true;
		
		_totalElapsed -= _frameInterval;

		//if (_totalElapsed > _frameInterval)
		//{
		//	SDL_Log("Performance warning, rendering or update took too long");
		//}
	}
	
	return result;
}
