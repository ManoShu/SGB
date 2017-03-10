#pragma once

#include "SGB.h"

class SGB_Screen;

class TestScreen :
	public SGB_Screen
{
public:

	virtual void LoadScreen() override;
	virtual void Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning) override;
	virtual void ScreenShow() override;

	float _elapsedCounter;
	SDL_Color _boxColor;

	const Uint8* keyStates;
};

