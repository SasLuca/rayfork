# Rayfork
A set of single header libraries forked from the awesome Raylib game framework: https://www.raylib.com/

Raylib was created initially for educational purposes and has matured over time into a very useful games library, similar to XNA. 
However, due to its nature, several design choices in Raylib make it hard to use for professional game developers: 
- Hard to use a custom platform layer (eg: using with a custom platform layer on Android with Android Studio)
- Hard to port on other platforms (eg: iOS, consoles)
- Little control over memory allocations and IO

The Rayfork header libraries are designed to address those issues and make it easy to develop professional games using the API from Raylib.

The headers are developed with the following principles in mind:

1. Provide platform-independent code.
The code in rayfork fully removes the platform layer from Raylib, instead the developer must provide one. Great care is also taken
to avoid hosted (os-dependent) function calls.

2. Provide more control over IO and memory.
Functions that do IO will remain as an option, but you will always have the choice to do the IO yourself.
We also aim to remove all uses of malloc/free in the code, instead asking the user of the library for buffers/allocators.

3. No global variables.
All global variables have been refactored into a struct that the user of the library can manage however they see fit.

4. Easy to build from source and include in projects.
The libraries will remain single header in order to make it easy for people to integrate them with their own build system.
All dependencies are listed and the developer can reason about them and decide whether to include them.

For more on the principles of how we develop these libraries check this article: https://handmade.network/wiki/7138-how_to_write_better_game_libraries

# Notice
Rayfork is still under very early development. If you plan to use it in a project be advised that there can be big changes over time
and that there can be bugs in the library resulted from the massive refactoring.
We will announce once we believe the library has reached a state where it can safely be used in production. 

# How to use
To use `rayfork_renderer.h` simply include the header and it's dependencies in your project and include the file like so:
```cpp
#include <your_opengl> //Include the opengl headers for your platform
#define RF_RENDERER_IMPL //You must define this in at least one .c/.cpp files to include the implementation
#define RF_GRAPHICS_API_OPENGL_33 //Choose a graphics API
#include "rayfork_renderer.h" //Include rayfork
```

For an actual example you can clone this project and compile it using the CMake file or using the batch script (only works on Windows with the msvc command line `cl`).
You can also open the project in CLion and compile and run it.

Currently there is a small example provided, a port of the geometric shapes example from Raylib, made using [sokol_app](https://github.com/floooh/sokol/blob/master/sokol_app.h) and [glad](https://glad.dav1d.de/) for the platform layer.
You can check it in `examples/geometric_shapes`.

# Current status

Currently only the Raylib renderer has been forked into `rayfork_renderer.h`.

Regarding `rayfork_renderer.h` this is a non-exhaustive list of changes and things to be aware of:
- Only the OpenGL33 and OpenglES2 graphics backends work. Opengl22 has issues with font rendering (at least with the default font) and the Opengl1 just doesn't work. 
- Support for VR was removed, might be added again later on.
- Input was removed since that is dependent on the platform layer.
- OpenGL is no longer loaded by the library, you must load it yourself. This was done because different platforms load OpenGL differently
and how it gets loaded heavily depends on your platform layer. There can also be multiple mutually exclusive ways to load OpenGL on just one platform.
- Async loading functions were added for certain asset types, we plan to support async loading for all asset types.
- The naming convention was changed to `rf_function_name` for public functions and `_rf_function_name` for private functions.
- All global variables were moved into a struct called `rf_context` which must be initialised by the user and passed by pointer to most functions.
- The user must provide the implementation for a couple of simple functions that rely on hosted functionality (eg: IO functions, time query functions, logging)
- Most text handling functions were removed.
- More functions now ask for the size of buffers.
- Library was tested and works on Windows, Android and iOS.

## Rayfork example running on iOS and Windows:
![](https://i.gyazo.com/26acc43576d3156852c25bf4c57ca1cf.png)
![](https://i.gyazo.com/thumb/1000/95dd519e8c6d6733acdb70f746a169fc-png.jpg)

We plan to also fork and apply a similar set of changes to:
- Raudio
- Rgif
- Rnet

# Contribute
If you want to be able to develop games easily with libraries that respect the principles mentioned above, please consider contributing to Rayfork.
You can check the issues tab and find a lot of things you can do to contribute.

I started this project because I love Raylib and I really wanted to develop my game using it. 
I love coding in C/C++ and I want to make games in low level languages.

Many libraries however do not follow the principles that I look for in a library (see [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries).
I want to create a set of libraries that indie developers can confidently and easily use to develop their projects without sacrificing control, portability or quality.
