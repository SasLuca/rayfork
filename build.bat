@echo off

SET INCLUDES=/I "../../dependencies/cgltf" /I "../../dependencies/dr_libs" /I "../../dependencies/jar" /I "../../dependencies/miniaudio" /I "../../dependencies/par" /I "../../dependencies/stb" /I "../../dependencies/tinyobjloader-c" /I "../../examples/dependencies/" /I "../../examples/dependencies/sokol" /I "../../rayfork"

::build all examples
if not exist bin mkdir bin
pushd bin

::basic shapes
if not exist basic_shapes mkdir basic_shapes
pushd basic_shapes
echo.
echo Building basic_shapes example
cl /nologo /Zi /Fe:basic_shapes %INCLUDES% /I "../../examples/basic_shapes/"  ../../examples/basic_shapes/main.c ../../examples/basic_shapes/game.c ../../examples/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

::collision area
if not exist collision_area mkdir collision_area
pushd collision_area
echo.
echo Building collision_area example
cl /nologo /Zi /Fe:collision_area %INCLUDES% /I "../../examples/collision_area/" ../../examples/collision_area/main.c ../../examples/collision_area/game.c ../../examples/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

::geometric_shapes
if not exist geometric_shapes mkdir geometric_shapes
pushd geometric_shapes
echo.
echo Building geometric_shapes example
cl /nologo /Zi /Fe:geometric_shapes %INCLUDES% /I "../../examples/geometric_shapes/" ../../examples/geometric_shapes/main.c ../../examples/geometric_shapes/game.c ../../examples/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

::hot code reloading
if not exist hot_code_reloading mkdir hot_code_reloading
pushd hot_code_reloading
echo.
echo Building hot_code_reloading example
cl /nologo /Zi /Fe:hot_code_reloading %INCLUDES% /I "../../examples/hot_code_reloading/" ../../examples/hot_code_reloading/win32_main.c /link user32.lib Gdi32.lib Opengl32.lib
cl /nologo /Zi /Fe:libhot_code_reloading_dynamic /LD %INCLUDES% /I "../../examples/hot_code_reloading/" ../../examples/hot_code_reloading/game.c ../../examples/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib /EXPORT:game_init /EXPORT:game_update /EXPORT:game_refresh
popd

::raw_audio_stream - Not working for some weird reason
::if not exist raw_audio_stream mkdir raw_audio_stream
::pushd raw_audio_stream
::echo.
::echo Building raw_audio_stream example
::cl /nologo /Zi /Fe:raw_audio_stream /I "../../dependencies" /I "../../rayfork" /I "../../examples/raw_audio_stream/" /I "../../examples/raw_audio_stream/dependencies" ../../examples/raw_audio_stream/main.c ../../examples/raw_audio_stream/game.c ../../examples/raw_audio_stream/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
::popd

popd