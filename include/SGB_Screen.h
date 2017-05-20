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
* <b>Example usage</b>:
* In this example, the screen loading process is simulated with a loop of SDL_Delay's,
* a grid of boxes equal to the average FPS is drawn and, after 10 seconds elapsed,
* the next screen is set (in this case is the same class, but can be any other SGB_Screen
* inherited class). If the user press ESC while the screen is running,
* `SGB_Display::StopRunning()` is called signalling the end of the main loop.

TestScreen.h
\includelineno TestScreen.h

TestScreen.cpp
\includelineno TestScreen.cpp

*/
class SGB_Screen
{
public:
	/*! \brief Creates an instance of SGB_Screen. */
	SGB_Screen();

	/*! \brief Destroys the SGB_Screen instance. */
	virtual ~SGB_Screen();

	/*! \brief Indicate to the SGB_Screen instance what instance of
	 * SGB_Display will handle it.
	*
	* \param display The SGB_Display that will handle the SGB_Screen
	* instance.
	*
	* The SGB_Screen::_display and SGB_Screen::_renderer are set at
	* this moment, making any other methods being able to access those
	* intances resources.
	*/
	void SetDisplay(SGB_Display* display);

	/*! \brief Loads any resouces needed by the SGB_Screen.
	*
	* This method should be used to load all resource-heavy,
	* time-consuming data, as this method and UnloadScreen() will be
	* called in a separate thread, while a loading screen (if set) is
	* shown during this process.
	*/
	virtual void LoadScreen() {};

	/*! \brief Prepare the SGB_Screen to be displayed.
	*
	* Unlike LoadScreen(), this method is executed in the main thread,
	* so it's recommended to keep it's processing load light and fast.
	*/
	virtual void ScreenShow() {};

	/*! \brief Process a new loop iteration.
	*
	* The SGB_Screen rendering will usually be done here.
	*/
	virtual void Update() = 0;

	/*! \brief Prepare the screen to be changed to another one.
	*
	* Use this method to stop any immediate processing the SGB_Screen
	* might be doing, like a network request or thread execution.
	*
	* This method is also run on the main thread, so it's wise to keep
	* it light and return fast.
	*/
	virtual void ScreenFinish() {};

	/*! \brief Unloads any resources used by the SGB_Screen.
	*
	* Any heavy resource release process, such as flushing logs, save
	* and closing file streams, should be done in this method, as, like
	* LoadScreen(), it runs on a separate thread from the caller one.
	*/
	virtual void UnloadScreen() {};

	/*! \brief Connects a SGB_LoadingScreen's SGB_LoadingQueue to this
	 * SGB_Screen.
	*
	* \param loadingScreen The SGB_LoadingScreen being used while this
	* SGB_Screen intance is loading.
	*
	* This method is not intended to be called manually, as it is
	* executed when the SGB_Display is preparing to load a new
	* SGB_Screen.
	*/
	void SetLoadingQueue(SGB_Screen* loadingScreen);

protected:

	/*! \brief Signals the SGB_Display to change SGB_Screen's.
	*
	* \param screen The next SGB_Screen to be shown.
	*
	* A shortcut that calls SGB_Display::SetScreen().
	*
	* <b>WARNING:</b> When calling SetNextScreen(), make sure
	* that there will be no more resources used until this screen's
	* current update loop ends, as ScreenFinish() and
	* UnloadScreen() will be called by the parent SGB_Display will
	* be called and there's no guarantee of the current loop
	* finishing before those calls.
	*/
	void SetNextScreen(SGB_Screen* screen);

	/*! \brief Builds a SDL_Color struct with the informed RGBA values.
	* \param r Red value (0-255)
	* \param g Green value (0-255)
	* \param b Blue value (0-255)
	* \param a Alpha value (0-255)
	*
	* \returns A SDL_Color structure with the given fields filled.
	*
	* The alpha parameter defaults to SDL_ALPHA_OPAQUE ( 0xff , 255 ).
	*/
	SDL_Color GetColor(Uint8 r, Uint8 g, Uint8 b,
		Uint8 a = SDL_ALPHA_OPAQUE);

	/*! \brief Sets the render color.
	*
	* \param color The SDL_Color to be set as render color.
	*
	* A shortcut that calls SGB_Display::SetDrawColor().
	*/
	void SetColor(SDL_Color color);

	/*! \brief Renders a filled rectangle with the given color and
	 * position/dimension.
	*
	* \param rect The SDL_Rect position\\dimension where the rectangle
	* will be rendered.
	* \param color The SDL_Color to be used to render the rectangle.
	*
	* Uses SGB_Display::GetDrawColor and SGB_Display::SetDrawColor to
	* temporarily set the render color and draw a "full" rectangle,
	* going back to the previous render color when done.
	*/
	void FillRect(SDL_Rect rect, SDL_Color color);

	/*! \brief Renders a non-filled rectangle, using the current draw
	 * color.
	 *
	 * \param rect The positions and size on where the rectangle will
	 * be drawn.
	 */
	void DrawRect(SDL_Rect rect);

	/*! \brief Renders a non-filled rectangle, using the given draw
	 * color.
	 *
	 * \param rect The positions and size on where the rectangle will
	 * be drawn.
	 * \param color The color to be used to draw the rectangle.
	 */
	void DrawRect(SDL_Rect rect, SDL_Color color);

	/*! \brief Renders a filled rectangle with the current render
	 * color and given position/dimension.
	*
	* \param rect The SDL_Rect position\\dimension where the rectangle
	* will be rendered.
	*/
	void FillRect(SDL_Rect rect);

	/*! \brief Gets the current renderer's width and height.
	*
	* \param[out] width The current renderer width.
	* \param[out] height The current renderer height.
	*
	* \returns <b>0</b> on success or a negative error code on failure;
	* call SDL_GetError() for more information.
	* [extracted from SDL's SDL_GetRendererOutputSize page]
	*
	* A shortcut that calls and returns
	* `SDL_GetRendererOutputSize(_renderer, width, height)`.
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

	/*! \brief Gets a SDL_Rect structure with then given x, y, width
	 * and height.
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
	* If SGB_Screen::_statusQueue is not NULL, the `status` message
	* will be put at the end of the queue, being accessed
	* by a SGB_LoadingScreen instance's
	* SGB_LoadingScreen::PullLoadingStatus method.
	*/
	void PushLoadingStatus(SGB_LoadingScreenStatus status);

	/*! \brief Holds the SGB_Display instance set by SetDisplay() */
	SGB_Display* _display;

	/*! \brief Holds the SDL_Renderer instance created by the
	 * SGB_Display set by SetDisplay() */
	SDL_Renderer* _renderer;

	/*! \brief Holds the SGB_LoadingQueue instance set by
	* SetLoadingQueue().
	*
	* The value is NULL if there is no SGB_LoadingScreen associated
	* with this SGB_Screen instance.
	*/
	SGB_LoadingQueue* _statusQueue;

};

