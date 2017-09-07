#include "SGB_DisplayLoadingManager.h"

#include "SGB_LoadingScreen.h"

SGB_DisplayLoadingManager::SGB_DisplayLoadingManager()
{
	_currentScreen =
		_currentLoadingScreen =
		_screenToBeUnloaded =
		_screenToBeLoaded =
		NULL;

	_loadingNextScreen = false;
	_finishedLoadingScreen.store(false);

	_loadingThread = NULL;
}

SGB_DisplayLoadingManager::~SGB_DisplayLoadingManager()
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
}

SGB_Screen* SGB_DisplayLoadingManager::GetCurrentScreen()
{
	return _currentScreen;
}

void SGB_DisplayLoadingManager::Update()
{
	if (_loadingNextScreen)
	{
		bool loadFinished = _finishedLoadingScreen.load();
		bool proceedLoading = false;

		if (_currentScreen != NULL)
		{
			auto loadingScreen = (SGB_LoadingScreen*)_currentScreen;
			
			proceedLoading = loadingScreen->
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
	else if (_screenToBeLoaded != NULL)
	{
		PrepareToLoad();
		StartLoadingProcess();
	}
}

void SGB_DisplayLoadingManager::SetOwner(SGB_Display * owner)
{
	_owner = owner;
}

void SGB_DisplayLoadingManager::SetScreen(SGB_Screen* screen)
{
	_screenToBeLoaded = screen;
}

void SGB_DisplayLoadingManager::SetLoadingScreen(SGB_Screen* screen)
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
		_currentLoadingScreen->SetDisplay(_owner);
		_currentLoadingScreen->LoadScreen();
	}
}

void SGB_DisplayLoadingManager::PrepareToLoad()
{
	_screenToBeUnloaded = _currentScreen;
	if (_screenToBeUnloaded != NULL)
	{
		_screenToBeUnloaded->ScreenFinish();
	}

	//_screenToBeLoaded = screen;

	_currentScreen = NULL;

	if (_currentLoadingScreen != NULL)
	{
		_currentScreen = _currentLoadingScreen;
		_currentLoadingScreen->ScreenShow();
	}
	
	_screenToBeLoaded->SetLoadingQueue(
		_currentLoadingScreen);

	_loadingNextScreen = true;

}

void SGB_DisplayLoadingManager::StartLoadingProcess()
{
	if (_loadingThread != NULL)
	{
		//delete _loadingThread;
		_loadingThread = NULL;
	}

	_loadingThread = SDL_CreateThread(
		SGB_DisplayLoadingManager::ExecuteLoadingProcess,
		"SGB_LoadingThread",
		(void*)this);
}

int SGB_DisplayLoadingManager::ExecuteLoadingProcess(void* data)
{
	auto t = (SGB_DisplayLoadingManager*)data;

	if (t->_screenToBeUnloaded != NULL)
	{
		t->_screenToBeUnloaded->UnloadScreen();
		delete t->_screenToBeUnloaded;
		t->_screenToBeUnloaded = NULL;
	}

	t->_screenToBeLoaded->SetDisplay(t->_owner);
	t->_screenToBeLoaded->LoadScreen();

	t->_finishedLoadingScreen.store(true);

	return 0;
}

void SGB_DisplayLoadingManager::FinishLoadingProcess()
{
	if (_screenToBeUnloaded != NULL)
	{
		delete _screenToBeUnloaded;
		_screenToBeUnloaded = NULL;
	}

	if (_loadingThread != NULL)
	{
		SDL_WaitThread(_loadingThread, NULL);
	}

	_currentScreen = _screenToBeLoaded;
	_currentScreen->ScreenShow();

	_screenToBeLoaded = NULL;

	_loadingNextScreen = false;
	_finishedLoadingScreen.store(false);
}
