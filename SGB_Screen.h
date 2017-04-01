#pragma once

#include "SGB_SDL.h"
#include "SGB_Display.h"
#include "SGB_LoadingQueue.h"

class SGB_Display;

/*! \brief Class to manage a screen flow, with loading, updating and unloading.
*
* The SGB_Screen class aims to reduce the effort needed to setup the rendering process
* and focus on the specific purpose of said screen.
*
* Example usage:
\code{.cpp}

//In this example, the screen loading process is simulated with a loop of SDL_Delay's,
//a grid of boxes equal to the average FPS is drawn and, after 10 seconds elapsed,
//the next screen is set (in this case is the same class, but can be any other SGB_Screen
//inherited class). If the user press ESC while the screen is running, isLoading is changed
//signalling to end the main loop.

//>>>TestScreen.h

#include "SGB.h"

class TestScreen :
	public SGB_Screen
{
public:

	virtual void LoadScreen() override;
	virtual void Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning) override;
	virtual void ScreenShow() override;

	float _elapsedCounter;
	SDL_Color _boxColor;

	const Uint8* keyStates;
};

//>>>TestScreen.cpp

#include "TestScreen.h"

void TestScreen::LoadScreen()
{
	//Getting states
	keyStates = SDL_GetKeyboardState(NULL);

	//color for the boxes
	_boxColor = GetColor(0xff, 0x40, 0x40);

	SGB_LoadingScreenStatus loadingStatus;

	for (int i = 0; i < 100; i++)
	{
		loadingStatus.progress = (float)(i / 100.0);
		sprintf(loadingStatus.status, "Loading %d %%...", i);
		PushLoadingStatus(loadingStatus);
		SDL_Delay(50);
	}

	loadingStatus.progress = 1;
	sprintf(loadingStatus.status, "Loading... Done.");
	PushLoadingStatus(loadingStatus);
}

void TestScreen::ScreenShow()
{
	_elapsedCounter = 0;
}

void TestScreen::Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool * isRunning)
{
	SDL_PumpEvents();

	if (keyStates[SDL_SCANCODE_ESCAPE])
	{
		*isRunning = false;
	}

	//Getting updated renderer dimensions
	int curWidth, curHeight;
	GetRendererSize(&curWidth, &curHeight);

	//drawing fps boxes
	SDL_Rect rect = GetRect(
		0,
		0,
		(int)((curWidth / 10.0) - 5),
		(int)((curHeight * 0.10) - 5));

	SetColor(_boxColor);

	int i = 0, j = 0;

	Uint32 fpsCounter = 0;

	while (fpsCounter < avgFPS)
	{
		if (i > 9)
		{
			i = 0;
			j++;
		}

		SetRect(&rect,
			(i * rect.w) + (i * 5),
			(j * rect.h) + (j * 5),
			rect.w,
			rect.h);

		FillRect(rect);

		i++;
		fpsCounter++;
	}


	_elapsedCounter += deltaT;

	//after 10 seconds has passed
	if (_elapsedCounter >= 10)
	{
		printf("(%f) - Changing screen\n", _elapsedCounter);
		SetNextScreen(new TestScreen());
	}
}

\endcode
*/
class SGB_Screen
{
public:
	/*! \brief Creates an instance of SGB_Screen. */
	SGB_Screen();

	/*! \brief Destroys the SGB_Screen instance. */
	virtual ~SGB_Screen();

	/*! \brief Indicate to the SGB_Screen instance what instance of SGB_Display will handle it.
	*
	* \param display The SGB_Display that will handle the SGB_Screen instance.
	*
	* The SGB_Screen::_display and SGB_Screen::_renderer are set at this moment,
	* making any other methods being able to access those intances resources.
	*/
	void SetDisplay(SGB_Display* display);

	/*! \brief Loads any resouces needed by the SGB_Screen.
	*
	* This method should be used to load all resource-heavy, time-consuming data,
	* as this method and UnloadScreen() will be called in a separate thread, while
	* a loading screen (if set) is shown during this process.
	*/
	virtual void LoadScreen() {};

	/*! \brief Prepare the SGB_Screen to be displayed.
	*
	* Unlike LoadScreen(), this method is executed in the main thread, so it's recommended
	* to keep it's processing load light and fast.
	*/
	virtual void ScreenShow() {};

	/*! \brief Process a new loop iteration.
	*
	* \param currentTime The time in ticks (ms) since the SGB_Display had been intialized.
	* \param elapsed The time in ticks (ms) since the last loop.
	* \param deltaT The time in seconds since the last loop.
	* \param avgFPS The last average FPS calculated
	* \param isRunning Set to <b>true</b> to signal the main loop to exit.
	*
	* The SGB_Screen rendering will usually be done here, given the parameters telling
	* how much time passed since the las iteration.
	*
	* The `isRunning` parameter can be set to true to normally signal the main loop to exit.
	*/
	virtual void Update(Uint32 currentTime, Uint32 elapsed, float deltaT, Uint32 avgFPS, bool* isRunning) = 0;

	/*! \brief Prepare the screen to be changed to another one.
	*
	* Use this method to stop any immediate processing the SGB_Screen might be doing,
	* like a network request or thread execution.
	*
	* This method is also run on the main thread, so it's wise to keep it light and return fast.
	*/
	virtual void ScreenFinish() {};

	/*! \brief Unloads any resources used by the SGB_Screen.
	*
	* Any heavy resource release process, such as flushing logs, save and closing file streams,
	* should be done in this method, as, like LoadScreen(), it runs on a separate thread from the caller one.
	*/
	virtual void UnloadScreen() {};

	/*! \brief Connects a SGB_LoadingScreen's SGB_LoadingQueue to this SGB_Screen.
	*
	* \param loadingScreen The SGB_LoadingScreen being used while this SGB_Screen intance is loading.
	*
	* This method is not intended to be called manually, as it is executed when
	* the SGB_Display is preparing to load a new SGB_Screen.
	*/
	void SetLoadingQueue(SGB_Screen* loadingScreen);

protected:

	/*! \brief Signals the SGB_Display to change SGB_Screen's.
	*
	* \param screen The next SGB_Screen to be shown.
	*
	* A shortcut that calls SGB_Display::SetScreen().
	*/
	void SetNextScreen(SGB_Screen* screen);

	/*! \brief Gets a SDL_Color structure with the given RGBA values.
	*
	* \param r Red value (0-255)
	* \param g Green value (0-255)
	* \param b Blue value (0-255)
	* \param a Alpha value (0-255)
	*
	* \returns A SDL_Color structure with the given fields filled.
	*
	* A shortcut that calls SGB_Display::GetColor().
	* The alpha parameter defaults to SDL_ALPHA_OPAQUE ( 0xff , 255 ).
	*/
	SDL_Color GetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE);

	/*! \brief Sets the render color.
	*
	* \param color The SDL_Color to be set as render color.
	*
	* A shortcut that calls SGB_Display::SetDrawColor().
	*/
	void SetColor(SDL_Color color);

	/*! \brief Renders a filled rectangle with the given color and position/dimension.
	*
	* \param rect The SDL_Rect position\\dimension where the rectangle will be rendered.
	* \param color The SDL_Color to be used to render the rectangle.
	*
	* Uses SGB_Display::GetDrawColor and SGB_Display::SetDrawColor to temporarily set the render color and
	* draw a "full" rectangle, going back to the previous render color when done.
	*/
	void FillRect(SDL_Rect rect, SDL_Color color);

	//TODO: doc
	void DrawRect(SDL_Rect rect);

	//TODO: doc
	void DrawRect(SDL_Rect rect, SDL_Color color);

	/*! \brief Renders a filled rectangle with the current render color and given position/dimension.
	*
	* \param rect The SDL_Rect position\\dimension where the rectangle will be rendered.
	*/
	void FillRect(SDL_Rect rect);

	/*! \brief Gets the current renderer's width and height.
	*
	* \param[out] width The current renderer width.
	* \param[out] height The current renderer height.
	*
	* \returns <b>0</b> on success or a negative error code on failure; call SDL_GetError() for more information.
	* [extracted from SDL's SDL_GetRendererOutputSize page]
	*
	* A short that calls and returns `SDL_GetRendererOutputSize(_renderer, width, height)`.
	*/
	int GetRendererSize(int* width, int* height);

	/*! \brief Set x, y, width and height of a SDL_Rect.
	*
	* \param[in,out] rect The SDL_Rect to be set.
	* \param x The X position.
	* \param y The Y position.
	* \param width The rectangle width.
	* \param height The rectangle height.
	*/
	void SetRect(SDL_Rect* rect, int x, int y, int width, int height);

	/*! \brief Gets a SDL_Rect structure with then given x, y, width and height.
	*
	* \param x The X position.
	* \param y The Y position.
	* \param width The rectangle width.
	* \param height The rectangle height.
	*
	* \returns A SDL_Rect structure with the given fields filled.
	*/
	SDL_Rect GetRect(int x, int y, int width, int height);

	/*! \brief Send a loading status update to the status queue.
	*
	* \param status The message status to be sent.
	*
	* If SGB_Screen::_statusQueue is not NULL, the `status` message will be put at the end of the queue, being accessed
	* by a SGB_LoadingScreen instance's SGB_LoadingScreen::PullLoadingStatus method.
	*/
	void PushLoadingStatus(SGB_LoadingScreenStatus status);

	/*! \brief Holds the SGB_Display instance set by SetDisplay() */
	SGB_Display* _display;

	/*! \brief Holds the SDL_Renderer instance created by the SGB_Display set by SetDisplay() */
	SDL_Renderer* _renderer;

	/*! \brief Holds the SGB_LoadingQueue instance set by SetLoadingQueue().
	*
	* The values is NULL if there is no SGB_LoadingScreen associated with this SGB_Screen instance.
	*/
	SGB_LoadingQueue* _statusQueue;

};

