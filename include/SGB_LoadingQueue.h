#pragma once

#include "SGB_types.h"
#include "SGB_Queue.h"

/*! \brief Internal class. A SGB_Queue to hold SGB_LoadingScreenStatus items between the loading thread and a loading screen. */
class SGB_LoadingQueue :
	public SGB_Queue<SGB_LoadingScreenStatus> {

};
