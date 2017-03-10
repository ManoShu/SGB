#include "TestDisplay.h"

SGB_DisplayInitInfo TestDisplay::GetInitInfo()
{
	SGB_DisplayInitInfo info;

	info.handleSDL = true;
	info.windowTitle = "Test Display";
	info.windowWidth = 160;
	info.windowHeight = 120;
	info.frameRate = 60;
	info.rendererIndex = 2;
	info.rendererFlags = SDL_RendererFlags::SDL_RENDERER_ACCELERATED;
	info.blendMode = SDL_BlendMode::SDL_BLENDMODE_NONE;
	info.enableVSync = true;
	info.borderless = false;
	info.fullScreen = false;
	info.unlockFPS = false;
	info.unlockOnBattery = false;
	info.fpsStep = 10;
	info.backgroundColor = GetColor(0xa0, 0xa0, 0xa0, 0xff);
	info.drawColor = GetColor(0x00, 0x00, 0x00, 0xff);

	return info;
}

void TestDisplay::BeginDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	SetDrawColor(_initInfo.drawColor);

	//Getting current renderer size
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	SDL_RenderDrawLine(_renderer, 0, curHeight / 2, curWidth, curHeight / 2);
	SDL_RenderDrawLine(_renderer, curWidth / 2, 0, curWidth / 2, curHeight);
}

void TestDisplay::EndDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	SetDrawColor(_initInfo.drawColor);

	//Getting current renderer size
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	SDL_RenderDrawLine(_renderer, 0, 0, curWidth, curHeight);
	SDL_RenderDrawLine(_renderer, 0, curHeight, curWidth, 0);
}
