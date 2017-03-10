#include "SGB_Screen.h"

SGB_Screen::SGB_Screen()
{
	_display = NULL;
}

SGB_Screen::~SGB_Screen()
{

}

void SGB_Screen::SetDisplay(SGB_Display* display)
{
	_display = display;
	_renderer = _display->GetRenderer();
}

void SGB_Screen::FillRect(SDL_Rect rect, SDL_Color color)
{
	SDL_Color oldColor = _display->GetDrawColor();

	_display->SetDrawColor(color);

	FillRect(rect);

	_display->SetDrawColor(oldColor);
}

void SGB_Screen::FillRect(SDL_Rect rect)
{
	SDL_RenderFillRect(_renderer, &rect);
}

int SGB_Screen::GetRendererSize(int * width, int * height)
{
	return SDL_GetRendererOutputSize(_renderer, width, height);
}

void SGB_Screen::SetRect(SDL_Rect * rect, int x, int y, int width, int height)
{
	rect->x = x;
	rect->y = y;
	rect->w = width;
	rect->h = height;
}

SDL_Rect SGB_Screen::GetRect(int x, int y, int width, int height)
{
	SDL_Rect rect;

	SetRect(&rect, x, y, width, height);

	return rect;
}

void SGB_Screen::SetNextScreen(SGB_Screen * screen)
{
	_display->SetScreen(screen);
}

SDL_Color SGB_Screen::GetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	return _display->GetColor(r, g, b, a);
}

void SGB_Screen::SetColor(SDL_Color color)
{
	_display->SetDrawColor(color);
}

void SGB_Screen::SetLoadingQueue(SGB_Screen* loadingScreen)
{
	if(loadingScreen == NULL)
	{
		_statusQueue = NULL;
	}
	else
	{
		_statusQueue = loadingScreen->_statusQueue;
	}
}

void SGB_Screen::PushLoadingStatus(SGB_LoadingScreenStatus status)
{
	if(_statusQueue != NULL)
	{
		_statusQueue->push(status);
	}
}
