@echo off

::build all examples
if not exist bin mkdir bin
pushd bin

::geometric_shapes
if not exist geometric_shapes mkdir geometric_shapes
pushd geometric_shapes
echo.
echo Building geometric_shapes example
cl /nologo /Zi /Fe:geometric_shapes /I "../../dependencies" /I "../../rayfork" /I "../../examples/geometric_shapes/" /I "../../examples/geometric_shapes/dependencies" ../../examples/geometric_shapes/main.c ../../examples/geometric_shapes/game.c ../../examples/geometric_shapes/rayfork_renderer.c ../../examples/geometric_shapes/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

::basic shapes
if not exist basic_shapes mkdir basic_shapes
pushd basic_shapes
echo.
echo Building basic_shapes example
cl /nologo /Zi /Fe:basic_shapes /I "../../dependencies" /I "../../rayfork" /I "../../examples/basic_shapes/" /I "../../examples/basic_shapes/dependencies" ../../examples/basic_shapes/main.c ../../examples/basic_shapes/game.c ../../examples/basic_shapes/rayfork_renderer.c ../../examples/basic_shapes/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

::collision area
if not exist collision_area mkdir collision_area
pushd collision_area
echo.
echo Building collision_area example
cl /nologo /Zi /Fe:collision_area /I "../../dependencies" /I "../../rayfork" /I "../../examples/collision_area/" /I "../../examples/collision_area/dependencies" ../../examples/collision_area/main.c ../../examples/collision_area/game.c ../../examples/collision_area/rayfork_renderer.c ../../examples/collision_area/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

popd