#pragma once

#include <atomic>

#include "SGB_SDL.h"

class SGB_Display;
class SGB_Screen;

class SGB_DisplayLoadingManager
{
public:
	SGB_DisplayLoadingManager();
	~SGB_DisplayLoadingManager();

	void SetOwner(SGB_Display* owner);
	void SetScreen(SGB_Screen* screen);
	void SetLoadingScreen(SGB_Screen* screen);
	void Update();
	SGB_Screen* GetCurrentScreen();

private:
	/*! \brief Start preparing the transition to a new `SGB_Screen`.
	*
	* \param screen The next SGB_Screen to be loaded.
	*
	* Here the current SGB_Screen (if exists) is set to be unloaded,
	* a `SGB_LoadingScreen` (if set) is set as the current screen.
	*/
	void PrepareToLoad(SGB_Screen* screen);

	/*! \brief Starts a thread calling `ExecuteLoadingProcess()`.
	*
	* This thread call passes the current instance to the satic method
	* being called.
	*/
	void StartLoadingProcess();

	/*! \brief Manages the loading steps to load and unload SGB_Screen`s
	* as necessary.
	*
	* This method unloads the previous screen, loads the next one and
	* flags _finishedLoadingScreen to <b>true</b> when finished.
	*/
	static int ExecuteLoadingProcess(void* data);

	/*! \brief Puts the newly loaded SGB_Screen as the current screen.
	*
	* The auxiliary fields are cleared and the new SGB_Screen is set
	* as current screen to be shown and updated.
	*/
	void FinishLoadingProcess();

	SGB_Display* _owner;

	//Stores the current SGB_Screen beign rendered at the moment
	SGB_Screen* _currentScreen;

	//Holds the instance of the current loading screen to be used during regular SGB_Screen changes
	SGB_Screen* _currentLoadingScreen;

	//Holds the instance of the last loaded SGB_Screen that will be unloaded to give place to the next one
	SGB_Screen* _screenToBeUnloaded;

	//Holds the instance of the next SGB_Screen that will have it's data loaded and then later
	//be set as the current screen.
	SGB_Screen* _screenToBeLoaded;

	//Indicates if a loading routine is in progress.
	bool _loadingNextScreen;

	//Indicates if the loading process finished.
	std::atomic<bool> _finishedLoadingScreen;

	//Holds the thread being used to execute the loading process
	SDL_Thread* _loadingThread;

};