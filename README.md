# Notice
Rayfork is still under very early development and it is not recommended that you use it at the moment.
You can check the rayfork 0.8 branch to see the active changes that are made as we are gearing towards a stable release.

# Rayfork
A set of single header libraries forked from the awesome Raylib game framework: https://www.raylib.com/

The headers are developed with the following principles in mind:

1. **Provide platform-independent code.**
The code in rayfork fully removes the platform layer from Raylib, instead the developer must provide one. Great care is also taken
to avoid hosted (os-dependent) function calls.

2. **Provide more control over IO and memory.**
Functions that do IO will remain as an option, but you will always have the choice to do the IO yourself.
We also aim to remove all uses of malloc/free in the code, instead asking the user of the library for buffers/allocators.

3. **Allow for hot code reloading.**
All global variables have been refactored into a struct that the user of the library can manage however they see fit.
The functions in rayfork use a global pointer to this context struct that the user can refresh if they do hot code reloading.

4. **Easy to build from source and include in projects.**
The libraries will remain single header in order to make it easy for people to integrate them with their own build system.
All dependencies are listed and the developer can reason about them and decide whether to include them.

For more on the principles of how we develop these libraries check this article: https://handmade.network/wiki/7138-how_to_write_better_game_libraries 

# How to use
To use rayfork simply include the header and it's dependencies in your project and include the file like so:
```cpp
#include <your_opengl> //Include the opengl headers for your platform
#define RF_RENDERER_IMPL //You must define this in at least one .c/.cpp files to include the implementation
#define RF_GRAPHICS_API_OPENGL_33 //Choose a graphics API
#define RF_AUDIO_IMPL //You must define this in at least one .c/.cpp files to include the implementation
#include "rayfork_audio.h" //Include rayfork audio (note: it's usually better to include rayfork-audio first)
#include "rayfork.h" //Include rayfork

rf_context rf_ctx; //Create a context variable
rf_audio_context rf_audio_ctx; //Create a context variable for rayfork-audio. Note that in the case of rayfork-audio the context struct must be in the same translation unit as the implementation

void init()
{
    rf_context_init(&rf_ctx, screen_width, screen_height); //init rayfork
    rf_load_font_default(); //Optionally load the default raylib font
    rf_audio_init(&rf_audio_ctx); //init rayfork-audio
}
```

For an actual example you can clone this project and compile it using the CMake file or using the batch scripts.
You can also open the project in CLion and compile and run it.
Several Raylib [examples](https://www.raylib.com/examples.html) have been ported to rayfork and more will be ported.

# Changes from Raylib

This is a non-exhaustive list of changes and things to be aware of:
- Only the OpenGL33 and OpenglES2 graphics backends work. Opengl22 has issues with font rendering (at least with the default font) and the Opengl1 just doesn't work. 
- Support for VR was removed, might be added again later on.
- Input was removed since that is dependent on the platform layer.
- OpenGL is no longer loaded by the library, you must load it yourself. This was done because different platforms load OpenGL differently
and how it gets loaded heavily depends on your platform layer. There can also be multiple mutually exclusive ways to load OpenGL on just one platform.
- Async loading functions were added for certain asset types, we plan to support async loading for all asset types.
- The naming convention was changed to `rf_function_name` for public functions and `_rf_function_name` for private functions.
- All global variables were moved into a struct called `rf_context` which must be allocated and initialised by the user. The functions use a global pointer to the `rf_context` struct that can be set by the user.
- The user must provide the implementation for a couple of simple functions that rely on hosted functionality (eg: IO functions, time query functions, logging)
- Most text handling functions were removed.
- More functions now ask for the size of buffers.
- Library was tested and works on Windows, MacOS, Linux, Android and iOS.

# Porting Raylib project to Rayfork

If you wrote a project in Raylib and want to move it over to Rayfork there are several things you might need to change:

1. You must provide your own platform layer which handle window creation and opengl loading. Examples of these include: sokol_app with glad, SDL, GLFW.
2. You must create a `rf_context` struct and use it to initialise the library.
3. You must change the names of the functions to match the Rayfork naming convention.
4. You must replace any functions from Raylib that were removed in Rayfork (eg: the text manipulation functions).

It is strongly advised that you review some of the examples to understand the changes made when moving a project from Raylib to Rayfork.

## Rayfork example running on iOS and Windows:
![](https://i.gyazo.com/a61b1fa44732a4cfbf4e7e59a2c5f772.png)
![](https://i.gyazo.com/thumb/1000/95dd519e8c6d6733acdb70f746a169fc-png.jpg)

We plan to also fork and apply a similar set of changes to:
- Rgif
- Rnet

# Rationale

Raylib was created initially for educational purposes and has matured over time into a very useful games library, similar to XNA. 
However, due to its nature, several design choices in Raylib make it hard to use for professional game developers: 
- Hard to use a custom platform layer (eg: using with a custom platform layer on Android with Android Studio)
- Hard to port on other platforms (eg: iOS, consoles)
- Little control over memory allocations and IO

The Rayfork header libraries are designed to address those issues and make it easy to develop professional games using the API from Raylib.

# Contribute
If you want to be able to develop games easily with libraries that respect the principles mentioned above, please consider contributing to Rayfork.
You can check the issues tab and find a lot of things you can do to contribute.

I started this project because I love Raylib and I really wanted to develop my game using it. 
I love coding in C/C++ and I want to make games in low level languages.

Many libraries however do not follow the principles that I look for in a library (see [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries)) which makes using them in games hard/annoying.
I want to create a set of libraries that indie developers can confidently and easily use to develop their projects without sacrificing control, portability or quality.

# Advice for contributors
- Keep the naming convention to snake case, use `rf_function_name` for interface functions and `_rf_function_name` for private functions.

- Prefix all functions with `RF_API` or `RF_INTERNAL`

- Don't include additional headers in the interface, work towards minimising includes in general.

- Use `//region` for folding regions of code. `//region` is already used throught the codebase so consider using an editor with support for folding those regions to get an easier grasp of the code.

- For porting Raylib examples, consider copying an existing example from the `examples` folder and just change the name. This is the fastest way to start working on this. The examples so far use `sokol_app` for the window management and GLAD for OpenGL loading.

- Try to apply the advice from [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries) in general. 
Some of the more important advice would be:
  - Don't allocate memory, ask the user for buffers/allocators.
  - Don't use non constant global variables.
  - Avoid os-dependent functions.

- If you want to use CLion there is a .idea folder provided.
