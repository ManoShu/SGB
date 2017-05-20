#include "SGB_Display.h"

#include "SGB_LoadingScreen.h"

#include <algorithm>
#include <thread>


SGB_Display::SGB_Display()
{
	_window = NULL;
	_renderer = NULL;

	_loadingManager.SetOwner(this);
	//_timingManager.SetOwner(this)

	_isRunning = true;
}

SGB_Display::~SGB_Display()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);

	if (_initInfo.HandleSDLStartupAndFinish)
	{
		SDL_Quit();
	}
}

int SGB_Display::Init()
{
	BeforeInit();

	if (_initInfo.HandleSDLStartupAndFinish)
	{
		if (SDL_Init(0))
		{
			return SGB_FAIL;
		}
	}

	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
	{
		if (SDL_InitSubSystem(SDL_INIT_VIDEO))
		{
			return SGB_FAIL;
		}
	}

	if (SDL_WasInit(SDL_INIT_TIMER) == 0)
	{
		if (SDL_InitSubSystem(SDL_INIT_TIMER))
		{
			return SGB_FAIL;
		}
	}

	_initInfo = GetInitInfo();

	Uint32 windowFlags = SDL_WINDOW_SHOWN;

	if (_initInfo.BorderlessWindow)
	{
		windowFlags |= SDL_WINDOW_BORDERLESS;
	}

	if (_initInfo.FullScreenWindow)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	_window = SDL_CreateWindow(
		_initInfo.WindowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_initInfo.WindowDefaultWidth,
		_initInfo.WindowDefaultHeight,
		windowFlags
	);

	if (_window == NULL)
	{
		return SGB_FAIL;
	}

	//checking battery status
	if (_initInfo.UnlockFrameRate && SDL_RunningOnBattery() && !_initInfo.UnlockFrameRateOnBattery)
	{
		_initInfo.UnlockFrameRate = false;
	}

	if (_initInfo.EnableVSync)
	{
		_initInfo.RendererFlags = _initInfo.RendererFlags | SDL_RENDERER_PRESENTVSYNC;
	}

	_renderer = SDL_CreateRenderer(_window, _initInfo.RendererIndex, _initInfo.RendererFlags);

	if (_renderer == NULL)
	{
		return SGB_FAIL;
	}

	if (SDL_SetRenderDrawBlendMode(_renderer, _initInfo.RendererBlendMode))
	{
		return SGB_FAIL;
	}

	if (_initInfo.FrameRateSamplesPerSecond < 1)
	{
		_initInfo.FrameRateSamplesPerSecond = 1;
	}

	_timingManager.Setup(_initInfo);

	AfterInit();

	_loopStats.TotalTicks = _timingManager.StartGlobalTimer();

	return SGB_SUCCESS;
}

void SGB_Display::Update()
{
	BeginUpdate();

	Update(_loadingManager.GetCurrentScreen());

	_loadingManager.Update();

	SDL_Event _displayEvent;

	if (SDL_PollEvent(&_displayEvent))
	{
		if (_displayEvent.type == SDL_QUIT)
		{
			StopRunning();
		}

		if (IsRunning())
		{
			SDL_PushEvent(&_displayEvent);
		}
	}

	EndUpdate();
}

SDL_Window* SGB_Display::GetWindow()
{
	return _window;
}

SDL_Renderer* SGB_Display::GetRenderer()
{
	return _renderer;
}

void SGB_Display::SetScreen(SGB_Screen* screen)
{
	_loadingManager.SetScreen(screen);
}

void SGB_Display::SetLoadingScreen(SGB_Screen* screen)
{
	_loadingManager.SetLoadingScreen(screen);
}

bool SGB_Display::IsRunning()
{
	return _isRunning;
}

void SGB_Display::StopRunning()
{
	_isRunning = false;
}

void SGB_Display::BeginUpdate()
{
	_loopStats.AverageFrameRate = _timingManager.CalculateAverageFPS();

	Clear();
}

void SGB_Display::Update(SGB_Screen* screen)
{
	_loopStats.TotalTicks = _timingManager.GetGlobalTicks();
	_loopStats.DeltaTicks = _timingManager.GetDeltaTicks();

	//Restart step timer so we can start counting again
	_timingManager.ResetDeltaTimer();

	_loopStats.DeltaSeconds = static_cast<float>(_loopStats.DeltaTicks) / 1000.f;

	//Note that the time information is NOT updated in each call
	BeginDraw();
	if (screen != NULL)
	{
		screen->Update();
	}
	EndDraw();

}

void SGB_Display::EndUpdate()
{
	UpdateDisplay();

	_timingManager.FinishCycle();
}

void SGB_Display::ResetDrawColor()
{
	SetDrawColor(_initInfo.RendererDefaultDrawColor);
}

void SGB_Display::SetDrawColor(SDL_Color color)
{
	SDL_SetRenderDrawColor(
		_renderer,
		color.r,
		color.g,
		color.b,
		color.a);
}

SDL_Color SGB_Display::GetDrawColor()
{
	SDL_Color color;

	SDL_GetRenderDrawColor(_renderer,
		&color.r,
		&color.g,
		&color.b,
		&color.a);

	return color;
}

SGB_DisplayInitInfo SGB_Display::GetDisplayInitInfo()
{
	return _initInfo;
}

SGB_DisplayLoopStats SGB_Display::GetLoopStats()
{
	return _loopStats;
}

void SGB_Display::Clear()
{
	SetDrawColor(_initInfo.RendererBackgroundColor);

	SDL_RenderClear(_renderer);

	ResetDrawColor();
}

void SGB_Display::UpdateDisplay()
{
	SDL_RenderPresent(_renderer);
}

bool SGB_Display::SDL_RunningOnBattery()
{
	return SDL_GetPowerInfo(NULL, NULL) == SDL_POWERSTATE_ON_BATTERY;
}


