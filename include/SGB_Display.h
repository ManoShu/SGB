#pragma once

#include <algorithm>
#include <atomic>
#include <vector>

#include "SGB_SDL.h"

#include "SGB_types.h"
#include "SGB_Screen.h"
#include "SGB_DisplayLoadingManager.h"
#include "SGB_DisplayTimingManager.h"

class SGB_Screen;

//TODO: Try to insert the code from the example application on the documentation

/*! \brief A class to initialize and manage SDL_Window, SDL_Renderer
 * and SGB_Screen's
*
* <b>Usage example</b>: In this example, the `SGB_Display` implementation
* will render perpendicular lines "behind" the screen and diagonal ones
* on the "front":

TestDisplay.h
\includelineno TestDisplay.h

TestDisplay.cpp
\includelineno TestDisplay.cpp
*/
class SGB_Display
{
public:

	/*! \brief Creates an instance of the SGB_Display.
	*
	* Nothing of note, only some default field value initialization.
	*/
	SGB_Display();

	/*! \brief Destroys the SGB_Display instance.
	*
	* All of the instances of the current screen, the loading screen
	* and the loading thread will be automatically destroyed.
	*/
	virtual ~SGB_Display();

	/*! \brief Initialize the display instance, creating all SDL
	 * related objects.
	*
	* \returns <b>SGB_SUCCESS</b> if there's no problem,
	* <b>SGB_FAIL</b> otherwise.
	*
	* Use SDL_GetError() to know what happened in case of failure.
	*/
	int Init();

	/*! \brief Update all display data and attached SGB_Screen's.
	*
	* Call regularly on your main loop in order to keep the current
	* SGB_Screen updated and check for any loading that can be
	* happening at the moment.
	*/
	void Update();

	/*! \brief Define what will be the next SGB_Screen to be shown.
	*
	*\param screen The SGB_Screen to be set.
	*
	* If there's a previous SGB_Screen object beign used,
	* it will automatically delete the instance.
	*
	* <b>WARNING:</b> When a SGB_Screen instance calls SetScreen(),
	* normally by calling SGB_Screen::SetNextScreen(), make sure
	* that there will be no more resources used until that screen's
	* current update loop ends, as SGB_Screen::ScreenFinish() and
	* SGB_Screen::UnloadScreen() will be called and there's no
	* guarantee of the current loop finishing before those calls.
	*
	* If a loading screen is set, it will be shown
	* until the \p screen object is fully loaded and
	* the loading screen instance set the signal to proceed.
	*
	* If there's no loading screen, a "blank" display will
	* shown, plus anything rendered by the `BeginDraw()` and `EndDraw()`
	*/
	void SetScreen(SGB_Screen* screen);

	/*! \brief Set the loading screen to be used during regular
	 * SGB_Screen's.
	*
	*\param screen The SGB_LoadingScreen to be set.
	*
	* Upon call, the loading screen's SGB_Screen::LoadScreen() will be
	* called to load any resources necessary by it.
	*
	* Note that this instance stays in memory between uses, beign
	* deleted when a new one is set or when the SGB_Display instance
	* is destroyed.
	*/
	void SetLoadingScreen(SGB_Screen* screen);

	/*! \brief Checks if the SGB_Display will keep running.
	*
	* This will return <b>true</b> until `StopRunning()` is called.
	*/
	bool IsRunning();

	/*! \brief Signalizes the SGB_Display to stop updating.
	*
	* This method does not free any resources, simply making
	* `IsRunning()` start returning <b>false</b>.
	*/
	void StopRunning();

	/*! \brief Gets the instance of SDL_Window created during `Init()`.
	*
	* \returns The SDL_Window instance created by `Init()`.
	*/
	SDL_Window* GetWindow();

	/*! \brief Gets the SDL_Renderer instance created during `Init()`
	*
	* \returns The SDL_Renderer instance created by Init().
	*/
	SDL_Renderer* GetRenderer();

	/*! \brief Set the current render color to the
	 * `SGB_DisplayInitInfo::RendererDefaultDrawColor` defined on `Init()` */
	void ResetDrawColor();

	/*! \brief Set the current render color to the one informed.
	*
	*\param color The SDL_Color to be used as current render color
	*/
	void SetDrawColor(SDL_Color color);

	/*! \brief Gets current render color.
	*
	* \returns The current render color.
	*/
	SDL_Color GetDrawColor();

	/*! \brief Gets a copy of the initialization data provided by
	 * `GetInitInfo()`.*/
	SGB_DisplayInitInfo GetDisplayInitInfo();

	/*! \brief Gets a copy of the current loop cycle stats.*/
	SGB_DisplayLoopStats GetLoopStats();

protected:

	/*! \brief Execute actions before `Init()` is run.
	 *
	 * Use this method when there is need to process data before
	 * building the SGB_DisplayInitInfo returned by `GetInitInfo()`,
	 * For example, one can loading the setting from a config file
	 * instead of hard-coding values.
	 */
	virtual void BeforeInit() {};

	/*! \brief Requests information about how the SGB_Display will be
	 * initialized.
	*
	* \returns The initialization info needed to complete Init().
	*
	* All classes inheriting from SGB_Display must provide the
	* initialization info so that `Init()` can be completed.
	*/
	virtual SGB_DisplayInitInfo GetInitInfo() = 0;

	/*! \brief Execute actions after the SGB_DisplayInitInfo has
	 * set on `GetInitInfo()` and `Init()` created the SDL_Window
	 * and SDL_Renderer instances.
	 *
	 * Use this method to load resources, especially ones that
	 * require passing the instances of either SDL_Window or
	 * SDL_Renderer.
	 */
	virtual void AfterInit() {};

	/*! \brief Allows to render BEFORE the current SGB_Screen
	 * (be a regular or loading screen) is rendered.
	*
	* Any rendering done in this moment, will appear "behind" what
	* the current screen will render.
	*/
	virtual void BeginDraw() {};

	/*! \brief Allows to render AFTER the current SGB_Screen
	 * (be a regular or loading screen) is rendered.
	*
	* Any rendering done in this moment, will appear "in front" of
	* what the current screen renderer.
	*/
	virtual void EndDraw() {};

	/*! \brief Holds the SDL_Window instance created by `Init()` */
	SDL_Window* _window;

	/*! \brief Holds the SDL_Renderer instance created by `Init()` */
	SDL_Renderer* _renderer;

	/*! \brief Holds the initialization info provided by
	* `GetInitInfo()` and used on `Init()` for later use.
	*
	* Internally, after `Init()` is run, provides the sampling size
	* for the FPS calculation
	* (SGB_DisplayInitInfo::FrameRateSamplesPerSecond)
	*/
	SGB_DisplayInitInfo _initInfo;

	/*! \brief Hold information related to the current loop cycle.
	*
	* This structure is updated on the start of each loop cycle,
	* storing data about that particular cycle, with the exception of
	* `SGB_DisplayLoopStats::AverageFPS`, which is updated when the
	* FPS calculation is done.
	*/
	SGB_DisplayLoopStats _loopStats;

private:

	/*! \brief Update cycle status and the current SGB_Screen.
	 *
	 * \param screen The current SGB_Screen.
	 *
	 * Here the calculations about the current loop cycle are done, as
	 * well the call for the current SGB_Screen's
	 * `SGB_Screen::Update()`.
	 */
	void UpdateScreen(SGB_Screen* screen);
	
	/*! \brief Render the current SGB_Screen.
	 *
	 * \param screen The current SGB_Screen.
	 *
	 * Here the drawing calls are done: starting with
	 * `BeginDraw()`, the current SGB_Screen's
	 * `SGB_Screen::Draw()` and then `EndDraw()`.
	 */
	void RenderScreen(SGB_Screen* screen);

	/*! \brief Fills the underlying renderer with the defined
	 * background color. */
	void Clear();

	/*! \brief Tries to check if the application is running on a device
	 * operation on batteries.
	 *
	 * This check is to not spend too much resources unlocking
	 * the framerate when there is a limited power supply, unless
	 * overriden.
	 */
	bool IsRunningOnBattery();

	/*! \brief Internal field to hold true until `StopRunning()` is
	 * called.
	 */
	bool _isRunning;

	/*! \brief Hold data related to the loading process between
	* SGB_Screen's.
	*/
	SGB_DisplayLoadingManager _loadingManager;

	/*! \brief Holds data related to loop cycle information. */
	SGB_DisplayTimingManager _timingManager;
};

