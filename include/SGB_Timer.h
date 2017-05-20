#pragma once

#include "SGB_SDL.h"

/*! \brief Internal class. A stopwatch\\timer using SDL_GetTicks() as time source.
*
* Based on http://lazyfoo.net/tutorials/SDL/23_advanced_timers/index.php
*/
class SGB_Timer
{
public:
	/*! \brief Initializes variables */
	SGB_Timer();

	//The various clock actions

	/*! \brief Start the timer. If already started, resets the base time. */
	void start();

	/*! \brief Stops the timer */
	void stop();

	/*! \brief Pause the timer, without resetting the base time */
	void pause();

	/*! \brief Unpause the timer, keeping the base time intact */
	void unpause();

	/*! \brief Gets the timer's time elaped between now and the base time
	*
	* \returns The time passed between the last start() call and now, in ticks (ms).
	*/
	Uint32 getTicks();

	/*! \brief Checks if the timer is stated
	*
	* \returns If the timer is currently started.
	*/
	bool isStarted();

	/*! \brief Checks if the timer is paused
	*
	* \returns If the timer is currently paused.
	*/
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};

