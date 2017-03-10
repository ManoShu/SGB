#pragma once

#include "SGB.h"

class TestDisplay :
	public SGB_Display
{
public:

	virtual SGB_DisplayInitInfo GetInitInfo() override;

	virtual void BeginDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning);
	virtual void EndDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning);
};

