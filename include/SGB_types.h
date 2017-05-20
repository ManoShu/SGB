#pragma once

#include <vector>
#include <atomic>

#include "SGB_Timer.h"

class SGB_Screen;

/*! \brief Indicates there was no problem executing a method. */
const int SGB_SUCCESS = 0;

/*! \brief Indicates that something went wrong executing a method.
*
* If it was caused by some SDL call, `SDL_GetError()` can be used to get a description of the error.
*/
const int SGB_FAIL = -1;

/*! \brief Defines a size for the loading screen message */
#ifndef SGB_LOADING_MESSAGE_SIZE
#define SGB_LOADING_MESSAGE_SIZE 255
#endif

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

struct SGB_DisplayLoopStats {
public:
	/*! \brief Stores how much time has passed since the SGB_Display initialization */
	Uint32 TotalTicks;

	/*! \brief Stores how many ticks has passed since the last loop cycle.
	*
	* Use this value when calculations based on miliseconds passed are needed.
	*/
	Uint32 DeltaTicks;

	/*! \brief Stores how many seconds has passed since the last loop cycle.
	*
	* Normally this value will be less than 1 second (< 1.0f).
	* Use this value when "something per second" calculations are needed.
	*/
	float DeltaSeconds;

	/*! \brief Stores the last calculated average frame rate*/
	Uint32 AverageFrameRate;

};

/*! \brief Structure to send the status of the loading process to an running SGB_LoadingScreen instance.
*
* BE CAREFUL: the `data` field will be passed from a thread to another. Keep that in mind.
*/
struct SGB_LoadingScreenStatus
{
	/*! \brief The current loading progress.
	*
	* A value from 0.0 to 1.0 indicating how much of the loading has been done.
	* \code{.cpp}
	*     item.progress = 0.62; //62% progress
	* \endcode
	*/
	float progress;

	/*! \brief A general message for what is currently happening on the loading process.
	*
	* A free string field to send more verbose information about what is being processed.
	* \code{.cpp}
	*     item.status = "Loading assets...";
	* \endcode
	*/
	char status[SGB_LOADING_MESSAGE_SIZE];

	/*! \brief A generic object holder.
	*
	* Use this field to pass something from the loading thread to the caller thread.
	* \code{.cpp}
	*     item.data = SDL_GetTicks();
	* \endcode
	*/
	void* data;
};
