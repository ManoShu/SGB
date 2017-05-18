#pragma once

#include "SGB_SDL.h"

#include "SGB_types.h" 
#include "SGB_Screen.h"
#include "SGB_Timer.h"

#include <algorithm>
#include <atomic> 
#include <vector>

class SGB_Screen;  

/*! \brief Scruture defining how a SGB_Display will be initialized.

Example base values:

\code{.cpp}

SGB_DisplayInitInfo info;

info.HandleSDLStartupAndFinish = true;
info.WindowTitle = "Window Title";
info.WindowDefaultWidth = 640;
info.WindowDefaultHeight = 480;
info.TargetFrameRate = 30;
info.RendererIndex = -1;
info.RendererFlags = 0;
info.RendererBlendMode = 0; //SDL_BlendMode::SDL_BLENDMODE_NONE;
info.EnableVSync = false;
info.BorderlessWindow = false;
info.FullScreenWindow = false;
info.UnlockFrameRate = false;
info.UnlockFrameRateOnBattery = false;
info.FrameRateSamplesPerSecond = 10;
//the GetColor functions comes from SGB_Display::SetColor()
info.RendererBackgroundColor = GetColor(0x00, 0x00, 0x00, 0xff);
info.RendererDefaultDrawColor = GetColor(0xff, 0xff, 0xff, 0xff);

\endcode
*/ 
struct SGB_DisplayInitInfo
{
public:

	/*! \brief Indicates if the SGB_Display will execute SDL_Init(0) on `SGB_Display::Init()` and SDL_Quit() on `SGB_Display::~SGB_Display()` */
	bool HandleSDLStartupAndFinish;

	/*! \brief Indicates what will be the title of the SDL_Window */
	const char* WindowTitle;

	/*! \brief Defines the initial width of the window */
	Uint32 WindowDefaultWidth;

	/*! \brief Defines the initial height of the window */
	Uint32 WindowDefaultHeight;

	/*! \brief Defines what is the target framerate for rendering (in `SGB_DisplayInitInfo::TargetFrameRate` frames per second).
	*
	* Based on various factors, the actual framerate can be lower if there's performance\\configuration issues.
	*
	* It also can be higher if `SGB_DisplayInitInfo::UnlockFrameRate` is set to true
	*/
	Uint32 TargetFrameRate;

	/*! \brief The renderer index to be passed to SDL_CreateRenderer.
	*
	* Use -1 to get the first one that supports the flags set on `SGB_DisplayInitInfo::RendererFlags`.
	*/
	Sint32 RendererIndex;

	/*! \brief The renderer flags to be passed to SDL_CreateRenderer.
	*
	* Use 0 to get the most basic (and possibly the most compatible) renderer possible.
	*
	* If `SGB_DisplayInitInfo::EnableVSync` is set to true, SDL_RENDERER_PRESENTVSYNC will be added to flags
	*/
	Sint32 RendererFlags;

	/*! \brief Defines what blend mode will be used by the SDL_Renderer created by `SGB_Display::Init()` */
	SDL_BlendMode RendererBlendMode;

	/*! \brief Indicates if the renderer must support VSync. */
	bool EnableVSync;

	/*! \brief Indicate if the SDL_Window created by `SGB_Display::Init()` will have no borders shown. */
	bool BorderlessWindow;

	/*! \brief Indicate if the SDL_Window created by `SGB_Display::Init()` will be open in fullscreen. */
	bool FullScreenWindow;

	/*! \brief Indicate if the `SGB_DisplayInitInfo::TargetFrameRate` must be ignored.
	*
	* If set to true, the SGB_Display will try to execute SGB_Display::Update() as soon as possible,
	* regardless of the value set on `SGB_DisplayInitInfo::TargetFrameRate`.
	*
	* This will not be taken effect if battery-based operation is detected and `UnlockFrameRateOnBattery` is not set to true.
	*/
	bool UnlockFrameRate;

	/*! \brief Indicates if the FPS should be unlocked, even when battery-based operation is detected.
	*
	* Running SGB_Display::Update() as soon is possible will use more processing power from the device
	* and with that, use more battery power.
	*
	* By default the FPS will not be unlocked unless this field is set to true.
	*/
	bool UnlockFrameRateOnBattery;

	/*! \brief Indicate at how many frames the FPS calculation must be done.
	*
	* The FPS calculation routine will be execute at each `(1000 / FrameRateSamplesPerSecond) ms`.
	*
	* Normally a reasonable value will be 10, giving a 100ms update window.
	*
	* A lower value will use less processing power, but it will take more time for the avarage to stabilize.
	* On the other side, a higher value will have a faster avarage update, but the ensuing processing cost
	* and possible average instability caused by the update window beign too small make it less viable.
	*/
	Uint32 FrameRateSamplesPerSecond;

	/*! \brief Indicate what is the background color of the display.
	*
	* Before each rendering loop, the SDL_Renderer area will be filled with this color.
	*/
	SDL_Color RendererBackgroundColor;

	/*! \brief Indicate the default rendering color of the display.
	*
	* Unless changed via SGB_Display::SetDrawColor(), any rendering will use this color as default.
	*/
	SDL_Color RendererDefaultDrawColor;
};

/*! \brief A class to initialize and manage SDL_Window, SDL_Renderer and SGB_Screen's
*
* Usage example:
*
\code{.cpp}
//In this example, the display will render perpendicular lines "behind" the screen and
//diagonal ones on the "front"

//>>>TestDisplay.h

#include "SGB.h"

class TestDisplay :
public SGB_Display
{
public:

	virtual SGB_DisplayInitInfo GetInitInfo() override;

	virtual void BeginDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning);
	virtual void EndDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning);
};

//>>>TestDisplay.cpp

#include "TestDisplay.h"

SGB_DisplayInitInfo TestDisplay::GetInitInfo()
{
	SGB_DisplayInitInfo info;

	info.HandleSDLStartupAndFinish = true;
	info.WindowTitle = "Test Display";
	info.WindowDefaultWidth = 320;
	info.WindowDefaultHeight = 240;
	info.TargetFrameRate = 60;
	info.RendererIndex = -1;
	info.RendererFlags = 0;
	info.RendererBlendMode = SDL_BlendMode::SDL_BLENDMODE_NONE;
	info.EnableVSync = true;
	info.BorderlessWindow = false;
	info.FullScreenWindow = false;
	info.UnlockFrameRate = false;
	info.UnlockFrameRateOnBattery = false;
	info.FrameRateSamplesPerSecond = 10;
	info.RendererBackgroundColor = GetColor(0xa0, 0xa0, 0xa0, 0xff);
	info.RendererDefaultDrawColor = GetColor(0x00, 0x00, 0x00, 0xff);

	return info;
}

void TestDisplay::BeginDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	SetDrawColor(_initInfo.RendererDefaultDrawColor);

	//Getting current renderer size
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	SDL_RenderDrawLine(_renderer, 0, curHeight / 2, curWidth, curHeight / 2);
	SDL_RenderDrawLine(_renderer, curWidth / 2, 0, curWidth / 2, curHeight);
}

void TestDisplay::EndDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	SetDrawColor(_initInfo.RendererDefaultDrawColor);

	//Getting current renderer size
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	SDL_RenderDrawLine(_renderer, 0, 0, curWidth, curHeight);
	SDL_RenderDrawLine(_renderer, 0, curHeight, curWidth, 0);
}

\endcode
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

	/*! \brief Initialize the display instance, creating all SDL related objects.
	*
	* \returns <b>SGB_SUCCESS</b> if there's no problem, <b>SGB_FAIL</b> otherwise.
	*
	* Use SDL_GetError() to know what happened in case of failure.
	*/
	int Init();

	/*! \brief Update all display data and attached SGB_Screen's.
	*
	*\param isRunning Set to <b>true</b> to signal the main loop to exit.
	*
	* Call regularly on your main loop in order to keep the current
	* SGB_Screen updated
	* and check for any loading that can be happening at the moment.
	*
	* The `isRunning` parameter can be used to indicate when to exit
	* the main loop
	*/
	void Update(bool* isRunning);

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

	/*! \brief Set the loading screen to be used during regular SGB_Screen's.
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

	/*! \brief Returns a SDL_Color struct with the informed RGBA values.
	* \param r Red value (0-255)
	* \param g Green value (0-255)
	* \param b Blue value (0-255)
	* \param a Alpha value (0-255)
	*
	* \returns A SDL_Color structure with the given fields filled.
	*/
	SDL_Color GetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE);

	/*! \brief Set the current render color to the `RendererDefaultDrawColor` defined on `Init()` */
	void SetDefaultColor();

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

protected:

	/*! \brief Execute actions before `Init()` is run.
	 * 
	 * Use this method when there is need to process data before
	 * building the SGB_DisplayInitInfo returned by `GetInitInfo()`,
	 * For example, one can loading the setting from a config file
	 * instead of hard-coding values.
	 */
	virtual void BeforeInit(){};

	/*! \brief Requests information about how the SGB_Display will be initialized.
	*
	* \returns The initialization info needed to complete Init().
	*
	* All classes inheriting from SGB_Display must provide the initialization info
	* so that `Init()` can be completed.
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
	virtual void AfterInit(){};

	/*! \brief Allows to render BEFORE the current SGB_Screen (be a regular or loading screen) is rendered.
	*
	* \param currentTime The time in ticks (ms) since the SGB_Display had been intialized.
	* \param elapsed The time in ticks (ms) since the last loop.
	* \param deltaT The time in seconds since the last loop.
	* \param avgFPS The last average FPS calculated
	* \param isRunning Set to <b>true</b> to signal the main loop to exit.
	*
	* Any rendering done in this moment, will appear "behind" what the current screen will render.
	*/
	virtual void BeginDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning) {};

	/*! \brief Allows to render AFTER the current SGB_Screen (be a regular or loading screen) is rendered.
	*
	* \param currentTime The time in ticks (ms) since the SGB_Display had been intialized.
	* \param elapsed The time in ticks (ms) since the last loop.
	* \param deltaT The time in seconds since the last loop.
	* \param avgFPS The last average FPS calculated
	* \param isRunning Set to <b>true</b> to signal the main loop to exit.

	* Any rendering done in this moment, will appear "in front" of what the current screen renderer.
	*/
	virtual void EndDraw(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning) {};

	/*! \brief Holds the SDL_Window instance created by `Init()` */
	SDL_Window* _window;

	/*! \brief Holds the SDL_Renderer instance created by `Init()` */
	SDL_Renderer* _renderer;

	/*! \brief Holds the initialization info provided by `GetInitInfo()` and used on `Init()` for later use.
	*
	* Internally, after `Init()` is run, provides the sampling size for the FPS calculation (SGB_DisplayInitInfo::FrameRateSamplesPerSecond)
	*/
	SGB_DisplayInitInfo _initInfo;

private:

	void BeginUpdate();
	void Update(SGB_Screen* menu, bool* isRunning);
	void EndUpdate();

	void PrepareToLoad(SGB_Screen * screen);
	void StartLoadingProcess();
	static int ExecuteLoadingProcess(void* data);
	void FinishLoadingProcess();


	void Clear();
	void UpdateDisplay();
	bool SDL_RunningOnBattery();

	//The frames per second timer
	SGB_Timer fpsTimer;

	//The frames per second cap timer
	SGB_Timer capTimer;

	//Timer to measure how much time has passed since the last =BeginUpdate= happened
	SGB_Timer stepTimer;

	//Counter to accumulate how many frames have passed since the last FPS sampling
	int countedFrames;

	//Stores when the last FPS sampling happened
	Uint32 lastCountReset;

	//Stores how much time is expected to pass between frame rendering and calculation
	int frameInterval;

	//Stores how much time has passed since the SGB_Display initialization
	Uint32 currentTime;

	//Stores the last calculated avarage FPS
	Uint32 avgFPS;

	//Store the last (_initInfo.FrameRateSamplesPerSecond) framecounts between FPS samplings
	std::vector<Uint32> _fpsQueue;

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

