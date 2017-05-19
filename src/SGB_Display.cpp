#include "SGB_Display.h"

#include "SGB_LoadingScreen.h"

#include <algorithm>
#include <thread>
#include <numeric>

SGB_Display::SGB_Display()
{
	_window = NULL;
	_renderer = NULL;

	_currentScreen =
		_currentLoadingScreen =
		_screenToBeUnloaded =
		_screenToBeLoaded =
		NULL;

	_loadingNextScreen = false;
	_finishedLoadingScreen.store(false);

	_loadingThread = NULL;

	_isRunning = true;
}

SGB_Display::~SGB_Display()
{
	if (_currentScreen != NULL)
	{
		_currentScreen->UnloadScreen();
		delete _currentScreen;
		_currentScreen = NULL;
	}

	if (_currentLoadingScreen != NULL)
	{
		_currentLoadingScreen->UnloadScreen();
		delete _currentLoadingScreen;
		_currentLoadingScreen = NULL;
	}

	if (_loadingThread != NULL)
	{
		//delete _loadingThread;
		_loadingThread = NULL;
	}

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

	frameInterval = (1000 / _initInfo.TargetFrameRate);
	countedFrames = 0;
	lastCountReset = 0;

	AfterInit();

	fpsTimer.start();

	_loopStats.TotalTicks = fpsTimer.getTicks();

	return SGB_SUCCESS;
}

void SGB_Display::Update()
{
	BeginUpdate();

	Update(_currentScreen);

	if (_loadingNextScreen)
	{
		bool loadFinished = _finishedLoadingScreen.load();
		bool proceedLoading = false;

		if (_currentScreen != NULL)
		{
			proceedLoading = ((SGB_LoadingScreen*)_currentScreen)->
				CheckLoading(loadFinished);
		}
		else
		{
			proceedLoading = true;
		}

		if (loadFinished && proceedLoading) {
			FinishLoadingProcess();
		}
	}

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

void SGB_Display::PrepareToLoad(SGB_Screen* screen)
{
	_screenToBeUnloaded = _currentScreen;
	if (_screenToBeUnloaded != NULL)
	{
		_screenToBeUnloaded->ScreenFinish();
	}

	_screenToBeLoaded = screen;

	_currentScreen = NULL;

	if (_currentLoadingScreen != NULL)
	{
		_currentLoadingScreen->ScreenShow();

		_currentScreen = _currentLoadingScreen;
	}

	_screenToBeLoaded->SetLoadingQueue(_currentLoadingScreen);

	_loadingNextScreen = true;

}

void SGB_Display::StartLoadingProcess()
{
	if (_loadingThread != NULL)
	{
		//delete _loadingThread;
		_loadingThread = NULL;
	}
	_loadingThread = SDL_CreateThread(
		SGB_Display::ExecuteLoadingProcess,
		"SGB_LoadingThread",
		(void*)this);
	//new std::thread(&SGB_Display::ExecuteLoadingProcess, this);
}

int SGB_Display::ExecuteLoadingProcess(void* data)
{
	auto t = (SGB_Display *)data;

	if (t->_screenToBeUnloaded != NULL)
	{
		t->_screenToBeUnloaded->UnloadScreen();
		delete t->_screenToBeUnloaded;
		t->_screenToBeUnloaded = NULL;
	}

	t->_screenToBeLoaded->SetDisplay(t);
	t->_screenToBeLoaded->LoadScreen();

	t->_finishedLoadingScreen.store(true);

	return 0;
}

void SGB_Display::FinishLoadingProcess()
{
	if (_screenToBeUnloaded != NULL)
	{
		delete _screenToBeUnloaded;
		_screenToBeUnloaded = NULL;
	}

	if (_loadingThread != NULL)
	{
		SDL_WaitThread(_loadingThread,NULL);
	}

	_currentScreen = _screenToBeLoaded;

	_currentScreen->ScreenShow();

	_screenToBeLoaded = NULL;

	_loadingNextScreen = false;
	_finishedLoadingScreen.store(false);
}

void SGB_Display::SetScreen(SGB_Screen * screen)
{
	PrepareToLoad(screen);
	StartLoadingProcess();
}

void SGB_Display::SetLoadingScreen(SGB_Screen* screen)
{
	if (_currentLoadingScreen != NULL)
	{
		_currentLoadingScreen->ScreenFinish();
		_currentLoadingScreen->UnloadScreen();
		delete _currentLoadingScreen;
	}

	_currentLoadingScreen = screen;

	if (_currentLoadingScreen != NULL)
	{
		_currentLoadingScreen->SetDisplay(this);
		_currentLoadingScreen->LoadScreen();
	}
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
	//Start cap timer
	capTimer.start();

	//Calculate and correct fps
	if (_initInfo.FrameRateSamplesPerSecond < 1)
	{
		_initInfo.FrameRateSamplesPerSecond = 1;
	}

	Uint32 split = (1000 / _initInfo.FrameRateSamplesPerSecond);
	Uint32 timeDiff = _loopStats.TotalTicks - lastCountReset;

	if (timeDiff > split)
	{
		lastCountReset = _loopStats.TotalTicks - (timeDiff - split);

		_fpsQueue.insert(_fpsQueue.end(), countedFrames);

		if (_fpsQueue.size() > _initInfo.FrameRateSamplesPerSecond)
		{
			_fpsQueue.erase(_fpsQueue.begin());
		}

		_loopStats.AverageFrameRate = std::accumulate(_fpsQueue.begin(), _fpsQueue.end(), (Uint32)0);

		countedFrames = 0;
	}

	Clear();
}

void SGB_Display::Update(SGB_Screen* menu)
{
	_loopStats.TotalTicks = fpsTimer.getTicks();
	_loopStats.DeltaTicks = stepTimer.getTicks();

	//Restart step timer so we can start counting again
	stepTimer.start();

	_loopStats.DeltaSeconds = static_cast<float>(_loopStats.DeltaTicks) / 1000.f;

	//Note that the time information is NOT updated in each call
	BeginDraw();
	if (menu != NULL)
	{
		menu->Update();
	}
	EndDraw();

}

void SGB_Display::EndUpdate()
{
	UpdateDisplay();

	++countedFrames;

	//If frame finished early
	int frameTicks = capTimer.getTicks();
	if (!_initInfo.UnlockFrameRate && frameTicks < frameInterval)
	{
		//Wait remaining time
		SDL_Delay(frameInterval - frameTicks);
	}
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


