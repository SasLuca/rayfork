@echo off

::build all examples
if not exist bin mkdir bin
pushd bin

::geometric_shapes
if not exist geometric_shapes mkdir geometric_shapes
pushd geometric_shapes
cl /nologo /Zi /Fe:geometric_shapes /I "../../dependencies" /I "../../rayfork" /I "../../examples/geometric_shapes/" /I "../../examples/geometric_shapes/dependencies" ../../examples/geometric_shapes/main.c ../../examples/geometric_shapes/game.c ../../examples/geometric_shapes/rayfork_renderer.c ../../examples/geometric_shapes/dependencies/glad/glad.c /link user32.lib Gdi32.lib Opengl32.lib
popd

popd