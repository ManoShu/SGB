#If you are not bound to SDL, go use [SFML](https://www.sfml-dev.org/), it's way more feature complete, organized and maintained. 

This library sucks compared to theirs and will not be updated anymore.

## SGB - SDL2 Graphical Bootstrapper

## Objective

To bootstrap any SDL2 graphical application, letting the user focus on what the application will actually do, instead of fiddling around rendering nuances.

## How to use

1) Clone the source

```
git clone https://github.com/ManoShu/SGB
```

### On Windows (as VC++ static library)

2) Download the latest Visual C++ 32bit binaries for SDL2

The zip file would be named `SDL2-devel-VERSION-VC.zip`

Create a `lib` folder on the cloned directory and unpack the zip file there.

Rename the `SDL2-VERSION` folder to just `SDL2`

Enter the SDL2 folder and move the `.h` files from the `include` folder to the parent folder.
Delete the now empty folder.

3) Build the solution

Open the SGB.sln file found in `build\msvc` and build it.
The lib file should be located on `build\msvc\bin\[Debug or Release]\SGB.lib`

### On Linux (using the included Makefile)

(Extra Step) if the SDL2 develepment libraries are not already installed,
you can either get it from your package manager (for apt, is `libsdl2-dev`),
or build and install it yourself. For a manual install:

* Download the SDL2 source code package, it should be named `SDL2-VERSION.tar.gz`, and unpack it

* Enter the `SDL-VERSION` directory and execute the following commands:

(This proccess assumes that gcc/g++ and automake are properly installed)

```
mkdir build
cd build
../configure
make
sudo make install
```

2) Build the source

```
cd SGB/build/makefile
make
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

#include "Implemented_SGB_Display .h"
#include "Implemented_SGB_Screen.h"
#include "Implemented_SGB_LoadingScreen.h"

int main(int argc, char** argv)
{
	Implemented_SGB_Display  myDisplay;

	if (myDisplay.Init() != SGB_SUCCESS)
	{
		printf("Failed to init: %s\n", SDL_GetError());
		return 1;
	}

	// a loading screen is not required, but it gives a better view for the end user
	myDisplay.SetLoadingScreen(new Implemented_SGB_LoadingScreen());

	myDisplay.SetScreen(new Implemented_SGB_Screen());

	while (myDisplay.IsRunning())
	{
		myDisplay.Update();
	}

	return 0;
}
```

### Example application

An simple application using the complete SGB library is 
contained in the `example` subdirectory.

##### Compile using the Makefile

Execute `make buildexample` on the `build/makefile` directory to build it. 
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

Just execute `make docs` from the `build/makefile` directory to generate a HTML version of the documentation.

The documentation will be located on the `doc/html` subdirectory. 
Open the `index.html` page to access all the generated documentation.

## Compiler Compatibility

The build process was tested on the following platforms:

* Custom ARM version based on Debian Linux on Next Thing Co.'s Pocket C.H.I.P., `uname -a` results in `Linux USER_NAME 4.4.13-ntc-mlc #1 SMP Thu Nov 3 01:28:54 UTC 2016 armv7l GNU/Linux` (Makefile)
* x86 Ubuntu installed on a Virtual Machine, uname result: `Linux USER_NAME 4.8.0-22-generic #24-Ubuntu SMP Sat Oct 8 09:14:42 UTC 2016 i686 i686 i686 GNU/Linux` (Makefile)
* x86 on Windows using Visual Studio 2015 Community, using SDL2 x86 pre-built binaries. Windows version 1607 (aka Windows 10)

## TODOs

* Documentation on the private section of the classes and methods;
* Check compatibility on other platforms (MinGW, OSX, Android, Raspberry PI);

## External Resources

* [SDL Main Page](https://libsdl.org/)
* [SDL Wiki](https://wiki.libsdl.org/)
