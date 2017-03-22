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

	if (_initInfo.handleSDL)
	{
		SDL_Quit();
	}
}

int SGB_Display::Init()
{
	BeforeInit();

	if (_initInfo.handleSDL)
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

	if (_initInfo.borderless)
	{
		windowFlags |= SDL_WINDOW_BORDERLESS;
	}

	if (_initInfo.fullScreen)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	_window = SDL_CreateWindow(
		_initInfo.windowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_initInfo.windowWidth,
		_initInfo.windowHeight,
		windowFlags
	);

	if (_window == NULL)
	{
		return SGB_FAIL;
	}

	//checking battery status
	if (_initInfo.unlockFPS && SDL_RunningOnBattery() && !_initInfo.unlockOnBattery)
	{
		_initInfo.unlockFPS = false;
	}

	if (_initInfo.enableVSync)
	{
		_initInfo.rendererFlags = _initInfo.rendererFlags | SDL_RENDERER_PRESENTVSYNC;
	}

	_renderer = SDL_CreateRenderer(_window, _initInfo.rendererIndex, _initInfo.rendererFlags);

	if (_renderer == NULL)
	{
		return SGB_FAIL;
	}

	if (SDL_SetRenderDrawBlendMode(_renderer, _initInfo.blendMode))
	{
		return SGB_FAIL;
	}

	frameInterval = (1000 / _initInfo.frameRate);
	countedFrames = 0;
	lastCountReset = 0;
	
	AfterInit();

	fpsTimer.start();
	currentTime = fpsTimer.getTicks();

	return SGB_SUCCESS;
}

void SGB_Display::Update(bool* isRunning)
{
	BeginUpdate();

	Update(_currentScreen, isRunning);

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
			*isRunning = false;
		}

		if (*isRunning == true)
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

void SGB_Display::BeginUpdate()
{
	//Start cap timer
	capTimer.start();

	//Calculate and correct fps
	if (_initInfo.fpsStep < 1)
	{
		_initInfo.fpsStep = 1;
	}

	Uint32 split = (1000 / _initInfo.fpsStep);
	Uint32 timeDiff = currentTime - lastCountReset;

	if (timeDiff > split)
	{
		lastCountReset = currentTime - (timeDiff - split);

		_fpsQueue.insert(_fpsQueue.end(), countedFrames);

		if (_fpsQueue.size() > _initInfo.fpsStep)
		{
			_fpsQueue.erase(_fpsQueue.begin());
		}

		avgFPS = std::accumulate(_fpsQueue.begin(), _fpsQueue.end(), (Uint32)0);

		countedFrames = 0;
	}

	Clear();
}

void SGB_Display::Update(SGB_Screen* menu, bool* isRunning)
{
	currentTime = fpsTimer.getTicks();

	Uint32 elapsed = stepTimer.getTicks();
	//Restart step timer so we can start counting again
	stepTimer.start();

	float deltaT = static_cast<float>(elapsed) / 1000.f;

	//Note that the time information is NOT updated in each call

	BeginDraw(currentTime, elapsed, deltaT, avgFPS, isRunning);

	if (menu != NULL)
	{
		menu->Update(currentTime, elapsed, deltaT, avgFPS, isRunning);
	}

	EndDraw(currentTime, elapsed, deltaT, avgFPS, isRunning);

}

void SGB_Display::EndUpdate()
{
	UpdateDisplay();

	++countedFrames;

	//If frame finished early
	int frameTicks = capTimer.getTicks();
	if (!_initInfo.unlockFPS && frameTicks < frameInterval)
	{
		//Wait remaining time
		SDL_Delay(frameInterval - frameTicks);
	}
}


SDL_Color SGB_Display::GetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_Color color;

	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;

	return color;
}

void SGB_Display::SetDefaultColor()
{
	SetDrawColor(_initInfo.drawColor);
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

void SGB_Display::Clear()
{
	SetDrawColor(_initInfo.backgroundColor);

	SDL_RenderClear(_renderer);

	SetDefaultColor();
}

void SGB_Display::UpdateDisplay()
{
	SDL_RenderPresent(_renderer);
}

bool SGB_Display::SDL_RunningOnBattery()
{
	return SDL_GetPowerInfo(NULL, NULL) == SDL_POWERSTATE_ON_BATTERY;
}


