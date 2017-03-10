#include "SGB_LoadingScreen.h"

SGB_LoadingScreen::SGB_LoadingScreen()
{
	_statusQueue = new SGB_LoadingQueue();
}

SGB_LoadingScreen::~SGB_LoadingScreen()
{
	if (_statusQueue == NULL)
	{
		delete _statusQueue;
		_statusQueue = NULL;
	}
}

int SGB_LoadingScreen::PullLoadingStatus(
	SGB_LoadingScreenStatus* status)
{
	if (_statusQueue != NULL && !_statusQueue->empty())
	{
		*status = _statusQueue->pop();
		return SGB_SUCCESS;
	}
	return SGB_FAIL;
}
