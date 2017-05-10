# SGB - SDL2 Graphical Bootstrapper

## Objective

To bootstrap any SDL2 graphical application, letting the user focus on what the application will actually do, instead of fiddling around rendering nuances.

## How to use

1) Clone the source

```
git clone https://github.com/ManoShu/SGB
```

2) Build the source

```
cd SGB/ && make
```

3) Install the library

```
sudo make install
```

4) Link the library

```
g++ YOUR_OTHER_PARAMETERS -lSGB
```

A typical main loop file will look like this:

```C++
#include <cstdio>
#include "SGB.h"

// put your SGB_Display, SGB_Screen and SGB_LoadingScreen implementation includes here

int main()
{
	bool isRunning = true;

	Implemented_SGB_Display myDisplay;

	if (myDisplay.Init() != SGB_SUCCESS)
	{
		printf("Failed to init: %s\n", SDL_GetError());
		return 1;
	}

	// a loading screen is not required, but it gives a better view for the end user
	myDisplay.SetLoadingScreen(new Implemented_SGB_LoadingScreen());

	myDisplay.SetScreen(new Implemented_SGB_Screen());

	while (isRunning)
	{
		myDisplay.Update(&isRunning);
	}
	
	return 0;
}
```

### Example application

An simple application using the complete SGB library is 
contained in the `example` subdirectory.

##### Compile using the Makefile

Execute `make buildexample` on the repository directory to build it. 
In addition to SGB beign compiled and installed, the SDL2 library and include files must be accesible 
for the compilation to succeed.

##### Compiling directly

Enter the `example` directory, then execute the following command to compile without a Makefile:

`g++ -o example *.cpp -lSGB -lSDL2 -std=c++11 `

##### Running the example

After a successful compilation, you can either execute `make runexample` (compiling the source on the fly) or 
enter de example directory and simple execute `./example` to run the application.
If it can't run properly, try changing some of the renderer options 
set on the `TestDisplay.cpp` file and build it again.

## Documentation

The comments on the files are [Doxygen](http://www.stack.nl/~dimitri/doxygen/download.html)-compatible, and a Doxyfile is provided with the base configuration.

Just execute `make docs` on the cloned directory to generate a HTML version of the documentation.

The documentation will be located on the `doc/html` subdirectory. 
Open the `index.html` page to access all the generated documentation.

## TODOs

* Documentation on the private section of the classes and the new few public methods;
* Review naming conventions for the entire project;
* Check compatibility of the build process on other platforms (vs-c++, mingw);

## External Resources

* [SDL Main Page](https://libsdl.org/)
* [SDL Wiki](https://wiki.libsdl.org/)
