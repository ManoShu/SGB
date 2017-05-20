#pragma once

#include "SGB_SDL.h"
#include "SGB_Screen.h"
#include "SGB_LoadingQueue.h"

class SGB_Screen;

/*! \brief A SGB_Screen used between regular SGB_Screen's loading process.
*
* <b>Example usage</b>:
* In this example, the loading screen will simply show a bar at the bottom
* of the screen based on the last progress status received and will
* automatically proceeed when the loading process is finished.

TestLoading.h
\includelineno TestLoading.h

TestLoading.cpp
\includelineno TestLoading.cpp

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
	*	while (PullLoadingStatus(&stats) == SGB_SUCCESS)
	*	{
	*		printf("Progress: %f, Status: %s\n", stats.progress, stats.status);
	*	}
	*\endcode
	*/
	int PullLoadingStatus(SGB_LoadingScreenStatus* status);
};

