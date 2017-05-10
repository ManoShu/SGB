#pragma once

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
