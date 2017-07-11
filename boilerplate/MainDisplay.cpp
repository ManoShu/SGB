#include "MainDisplay.h"

SGB_DisplayInitInfo MainDisplay::GetInitInfo()
{
	SGB_DisplayInitInfo info;

	info.HandleSDLStartupAndFinish = true; //SGB_Display will manage SDL for you
	info.WindowTitle = "Main Display";
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
	//The SDL_Color field are associated with a {r, g, b, a} set
	info.RendererBackgroundColor = { 0xa0, 0xa0, 0xa0, 0xff };
	info.RendererDefaultDrawColor = { 0x00, 0x00, 0x00, 0xff };

	return info;
}

void MainDisplay::BeginDraw()
{

}

void MainDisplay::EndDraw()
{

}
