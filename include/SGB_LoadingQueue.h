#pragma once

#include "SGB_types.h"
#include "SGB_Queue.h"

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

/*! \brief A SGB_Queue to hold SGB_LoadingScreenStatus items between the loading thread and a loading screen. */
class SGB_LoadingQueue :
	public SGB_Queue<SGB_LoadingScreenStatus> {

};