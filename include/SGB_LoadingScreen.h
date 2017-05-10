#pragma once

#include "SGB_SDL.h"
#include "SGB_Screen.h"
#include "SGB_LoadingQueue.h"

class SGB_Screen;

/*! \brief A SGB_Screen used between regular SGB_Screen's loading process.
*
* Example usage:
\code{.cpp}
//In this example, the loading screen will simply show a bar at the bottom
//of the screen based on the last progress status received and will
//automatically proceeed when the loading process is finished.

//>>>SampleLoading.h

#include "SGB.h"

class SampleLoading :
	public SGB_LoadingScreen
{

	private:

	virtual void ScreenShow() override;
	virtual void Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning) override;

	virtual bool CheckLoading(bool loadingFinished) override;

	SDL_Rect _rect;
	SDL_Color _rectColor;

	float _lastProgress;
};

//>>>SampleLoading.cpp

#include "SampleLoading.h"

void SampleLoading::ScreenShow()
{
	//A very noticeable Fuchsia
	_rectColor = _display->GetColor(0xff, 0x00, 0xff, SDL_ALPHA_OPAQUE);
	_lastProgress = 0;
}

void SampleLoading::Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	//Checking and processing loading status
	SGB_LoadingScreenStatus stats;
	if (PullLoadingStatus(&stats) == SGB_SUCCESS)
	{
		printf("Progress: %f, Status: %s\n", stats.progress, stats.status);
		_lastProgress = stats.progress;
	}

	//Getting updated renderer dimensions
	int curWidth, curHeight;
	SDL_GetRendererOutputSize(_renderer, &curWidth, &curHeight);

	//The loading rect size will be:
	//horizontal: 90% width + 5% margin on each side
	//vertical: 10% height and 5% bottom margin
	_rect.x = (int)(curWidth * 0.05);
	_rect.h = (int)(curHeight * 0.10);
	_rect.y = (int)(curHeight - _rect.h - (curHeight * 0.05));
	_rect.w = (int)((curWidth * 0.9) * _lastProgress);

	_display->SetDrawColor(_rectColor);

	SDL_RenderFillRect(_renderer, &_rect);
}

bool SampleLoading::CheckLoading(bool loadingFinished)
{
	if (loadingFinished)
	{
		return true;
	}

	return false;
}

\endcode
*/
class SGB_LoadingScreen :
	public SGB_Screen
{
public:

	/*! \brief Create a loading screen instance and instance the loading status queue. */
	SGB_LoadingScreen();

	/*! \brief Destroys the loading screen instance, along with the loading status queue. */
	virtual ~SGB_LoadingScreen();

	/*! \brief Asks the loading screen if the loading process can proceed.
	*
	* \param loadingFinished Indicates if the loading process was completed.
	*
	* The loading process will only "end" when `loadingFinished` is true
	* and the return value of this function is also true.
	*
	* Refrain from returning <b>true</b> before `loadingFinished`
	* is also true to avoid problems trying to show a SGB_Screen that could not have all
	* resources loaded yet.
	*
	*\code{.cpp}
	*	//Returns immediately after the loading finishes
	*	if (loadingFinished)
	*	{
	*		return true;
	*	}
	*	return false;
	*\endcode
	*/
	virtual bool CheckLoading(bool loadingFinished) = 0;

protected:

	/*! \brief Check and get the next possible loading status
	*
	* \param[out] status The next status, if there is any on the queue.
	*
	* Returns SGB_SUCCESS if there is a new status message, that is set on `status`.
	*\code{.cpp}
	*	if (PullLoadingStatus(&stats) == SGB_SUCCESS)
	*	{
	*		printf("Progress: %f, Status: %s\n", stats.progress, stats.status);
	*	}
	*\endcode
	*/
	int PullLoadingStatus(SGB_LoadingScreenStatus* status);
};

