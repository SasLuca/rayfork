<img src="https://sasluca.github.io/rayfork-logo.jpg" width="256">

# rayfork

A single header and source, cross-platform, XNA-like, allocator-aware game dev library. 

Forked from the awesome raylib game framework: https://www.raylib.com/

**Notice:** rayfork is still under very early development and it is not recommended that you use it professionally at the moment.

## How to build

rayfork only has one .c file and only depends on libc, which means it can be easily compiled as a library from the command line.

```shell script
# -c compiles the code as a library
# -EHsc disables exceptions on msvc

gcc -c rayfork.c
clang -c rayfork.c
cl -c -EHsc rayfork.c
```

You can also try it using this cmake template: https://github.com/SasLuca/rayfork-sokol-template

## Principles

### 1. Provide platform-independent code

rayfork does not provide a platform layer, that means it won't create a window, load OpenGL, or capture input for you.

This is by design, so that you can easily use rayfork on multiple platforms (including game consoles) by using the method that works best for you. 
There are templates for using rayfork with GLFW, SDL, sokol-app and custom platform layers.

The renderer currently has OpenGL33 and OpenGL-ES3 backends (with more to be added) that are implemented in a portable way which allows rayfork to be compiled on any platform, 
with the only dependency being libc. OpenGL procs are passed explicitly to rayfork and there is a simple macro to aid with this.  

Because of this you can easily compile rayfork for any platform be it PC, Mobile or Consoles.

### 2. Provide full control over IO and memory

Functions that do IO are often optional and explicitly ask for IO callbacks. A simple wrapper for the libc IO functions is provided as `RF_DEFAULT_IO`.

Functions that allocate explicitly ask for an allocator and sometimes also for a temporary memory allocator (memory that is freed inside the function). A simple wrapper for libc's malloc/free is provided as `RF_DEFAULT_ALLOCATOR`.

All dependencies are also used with custom allocators in mind, the library will never allocate without you knowing.

Every function that requires an allocator or io callbacks has an `_ez` version which wraps the original function and calls it with `RF_DEFAULT_ALLOCATOR` and/or `RF_DEFAULT_IO`, this is useful for testing code quickly.

### 3. Easy to build

The library is only one header and source file and can be customized at compile time using preprocessor definitions. No additional compile flags are needed, depending on the graphics backend you might need to link against certain libraries.

## Rationale

raylib was created initially for educational purposes and has matured over time into a very useful games library, similar to XNA. 

However, due to its nature, several design choices in raylib make it hard to use for professional game developers: 

- Hard to use a custom platform layer (eg: using with a custom platform layer on Android with Android Studio)

- Hard to port on other platforms (eg: iOS, consoles)

- Little control over memory allocations and io.

rayfork is designed to address those issues and make it easy to develop professional games using the API from raylib.

I started this project because I love raylib and C99 and I really wanted to develop my game using them.

Many libraries however do not follow the principles that I look for in a library (see [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries)) which makes using them in games hard/annoying which is why I want to create a library that indie developers can confidently and easily use to develop their projects without sacrificing control, portability or quality.

## Help needed

If you want to be able to develop games easily with libraries that respect the principles mentioned above, please consider contributing to rayfork.

You can check the issues tab and find a lot of things you can do to contribute.

I am also looking for help in developing things outside my expertise:
- Improving the rendering API
- More graphics backends (sokol-gfx, vulkan, custom console backends)
- A particle system
- Physics
- Networking

## Advice for contributors
- Contact me on the [raylib discord server](https://discord.gg/mzCY3wN) in the #rayfork channel, I am @BananyaDev#0001, or on [twitter @SasLuca](https://twitter.com/SasLuca).

- Keep the naming convention to snake case, use `rf_function_name` for interface functions and `_rf_function_name` for private functions.

- Prefix all functions with `RF_API` or `RF_INTERNAL`

- Don't include additional headers in the interface, work towards minimizing includes in general.

- Use `#pragma region \ #pragma endregion` for folding regions of code and consider using an editor with support for folding those regions to get an easier grasp of the code.

- Try to apply the advice from [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries) in general. 
Some of the more important advice would be:
  - Don't allocate memory, ask the user for vertex_buffers/allocators.
  - Don't use non constant global variables.
  - Avoid os-dependent functions.

## rayfork example running on iOS and Windows:

![](https://i.gyazo.com/a61b1fa44732a4cfbf4e7e59a2c5f772.png)
![](https://i.gyazo.com/thumb/1000/95dd519e8c6d6733acdb70f746a169fc-png.jpg)