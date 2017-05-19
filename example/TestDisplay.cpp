#include "TestDisplay.h"

SGB_DisplayInitInfo TestDisplay::GetInitInfo()
{
	SGB_DisplayInitInfo info;

	info.HandleSDLStartupAndFinish = true; //SGB_Display will manage SDL for you
	info.WindowTitle = "Example Display";
	info.WindowDefaultWidth = 300;
	info.WindowDefaultHeight = 200;
	info.TargetFrameRate = 60;// in frames per second
	info.RendererIndex = -1; //auto-select renderer
	info.RendererFlags = 0; //no renderer requirements
	//for more simple and fast rendering
	info.RendererBlendMode = SDL_BlendMode::SDL_BLENDMODE_NONE;
	info.EnableVSync = false;
	info.BorderlessWindow = false;
	info.FullScreenWindow = false;
	info.UnlockFrameRate = false;
	info.UnlockFrameRateOnBattery = false;
	info.FrameRateSamplesPerSecond = 10;
	info.RendererBackgroundColor = { 0xa0, 0xa0, 0xa0, 0xff };
	info.RendererDefaultDrawColor = { 0x00, 0x00, 0x00, 0xff };

	return info;
}

void TestDisplay::BeginDraw()
{
	ResetDrawColor();

	//Getting current renderer size
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	SDL_RenderDrawLine(_renderer, 0, curHeight / 2, curWidth, curHeight / 2);
	SDL_RenderDrawLine(_renderer, curWidth / 2, 0, curWidth / 2, curHeight);
}

void TestDisplay::EndDraw()
{
	ResetDrawColor();

	//Getting current renderer size
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	SDL_RenderDrawLine(_renderer, 0, 0, curWidth, curHeight);
	SDL_RenderDrawLine(_renderer, 0, curHeight, curWidth, 0);
}
