#pragma once

#include <vector>
#include "SGB_Timer.h"
#include "SGB_types.h"

class SGB_DisplayTimingManager
{
public:
	SGB_DisplayTimingManager();
	~SGB_DisplayTimingManager();

	void Setup(SGB_DisplayInitInfo info);

	Uint32 StartGlobalTimer();

	Uint32 CalculateAverageFPS(Uint32 currentTicks);

	Uint32 GetGlobalTicks();
	Uint32 GetDeltaTicks();

	void ResetDeltaTimer();

	void FinishCycle();

private:
	//The frames per second timer
	SGB_Timer fpsTimer;

	//The frames per second cap timer
	SGB_Timer capTimer;

	//Timer to measure how much time has passed since the last =BeginUpdate= happened
	SGB_Timer stepTimer;

	//Counter to accumulate how many frames have passed since the last FPS sampling
	int countedFrames;

	//Stores when the last FPS sampling happened
	Uint32 lastCountReset;

	//Stores how much time is expected to pass between frame rendering and calculation
	Uint32 frameInterval;

	//Store the last (_initInfo.FrameRateSamplesPerSecond) framecounts between FPS samplings
	std::vector<Uint32> _fpsQueue;

	SGB_DisplayInitInfo _initInfo;

	Uint32 _currentAverageFPS;
};

