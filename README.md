<img align="left" src="https://github.com/SasLuca/rayfork/blob/master/logo.jpg" width=256>

# rayfork
A single header, cross platform, allocator-aware game dev library forked from the awesome raylib game framework: https://www.raylib.com/

The goal is to have a powerful and easy to use XNA-like library for professional indie game development in C99.

**Notice:** rayfork is still under very early development and it is not recommended that you use it professionally at the moment.

<br>

# Principles

## 1. Provide platform-independent code

The code in rayfork fully removes the platform layer from Raylib, instead the developer must provide one. Great care is also taken to avoid hosted (os-dependent) function calls.

The renderer currently has OpenGL33 and OpenGL-ES3 backends (with more to be added) that are implemented in a portable way which allows rayfork to be compiled on any platform, with the only dependency being libc. OpenGL procs are passed explicitly to rayfork and there is a simple macro to aid with this.

Because of this you can easily compile rayfork for any platform be it PC, Mobile or Consoles.

## 2. Provide full control over IO and memory

Functions that do IO are often optional and explicitly ask for IO callbacks. A simple wrapper for the libc IO functions is provided as `RF_DEFAULT_IO`.

Functions that allocate explicitly ask for an allocator and sometimes also for a temporary memory allocator (memory that is freed by inside the function). A simple wrapper for libc's malloc/free is provided as `RF_DEFAULT_ALLOCATOR`.

All dependencies are also used with custom allocators in mind, the library should never allocate without you knowing.

## 3. Easy to build

The library is developed as multiple files that are concatenated into a single header version and a single header + single source version.

All dependencies are bundled inside of rayfork and marked `static`.

The library can be customized at compile time using preprocessor definitions.

## 4. Avoiding globals

All state required by rayfork is stored in one struct which the user must allocate, but functions in rayfork use a global pointer to this context struct that the user can set if they do hot code reloading.

For more on the principles of how we develop these libraries check this article: https://handmade.network/wiki/7138-how_to_write_better_game_libraries 

# Rationale

raylib was created initially for educational purposes and has matured over time into a very useful games library, similar to XNA. 

However, due to its nature, several design choices in raylib make it hard to use for professional game developers: 

- Hard to use a custom platform layer (eg: using with a custom platform layer on Android with Android Studio)

- Hard to port on other platforms (eg: iOS, consoles)

- Little control over memory allocations and IO

rayfork is designed to address those issues and make it easy to develop professional games using the API from raylib.

I started this project because I love raylib and I really wanted to develop my game using it. 

I love coding in C99 and I want to make games in low level languages.

Many libraries however do not follow the principles that I look for in a library (see [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries)) which makes using them in games hard/annoying which is why I want to create a library that indie developers can confidently and easily use to develop their projects without sacrificing control, portability or quality.

# Help needed

If you want to be able to develop games easily with libraries that respect the principles mentioned above, please consider contributing to rayfork.

You can check the issues tab and find a lot of things you can do to contribute.

## Advice for contributors
- Keep the naming convention to snake case, use `rf_function_name` for interface functions and `_rf_function_name` for private functions.

- Prefix all functions with `RF_API` or `RF_INTERNAL`

- Don't include additional headers in the interface, work towards minimizing includes in general.

- Use `#pragma region \ #pragma endregion` for folding regions of code and consider using an editor with support for folding those regions to get an easier grasp of the code.

- Try to apply the advice from [this article](https://handmade.network/wiki/7138-how_to_write_better_game_libraries) in general. 
Some of the more important advice would be:
  - Don't allocate memory, ask the user for buffers/allocators.
  - Don't use non constant global variables.
  - Avoid os-dependent functions.

## Rayfork example running on iOS and Windows:

![](https://i.gyazo.com/a61b1fa44732a4cfbf4e7e59a2c5f772.png)
![](https://i.gyazo.com/thumb/1000/95dd519e8c6d6733acdb70f746a169fc-png.jpg)
