#pragma once

#include "SGB.h"

class TestLoading :
	public SGB_LoadingScreen
{

private:

	virtual void ScreenShow() override;
	virtual void Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning) override;

	virtual bool CheckLoading(bool loadingFinished) override;

	SDL_Rect _rect;
	SDL_Color _rectColor;

	float _lastProgress;
};

