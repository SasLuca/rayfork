#!/bin/bash

FLAGS="-g -D_DEFAULT_SOURCE"
INCLUDES="-I../../rayfork -I../../dependencies -I../../examples/geometric_shapes/ -I../../examples/geometric_shapes/dependencies"
LIBRARIES="-lGL -lm -lpthread -ldl -lrt -lX11"

mkdir -p bin
pushd bin

mkdir -p basic_shapes
pushd basic_shapes
echo -e '\n'
echo Building basic_shapes example
gcc -o basic_shapes ../../examples/basic_shapes/main.c ../../examples/basic_shapes/game.c ../../examples/basic_shapes/rayfork_renderer.c ../../examples/basic_shapes/dependencies/glad/glad.c $FLAGS $INCLUDES $LIBRARIES;
popd

mkdir -p collision_area
pushd collision_area
echo -e '\n'
echo Building collision_area example
gcc -o collision_area ../../examples/collision_area/main.c ../../examples/collision_area/game.c ../../examples/collision_area/rayfork_renderer.c ../../examples/collision_area/dependencies/glad/glad.c $FLAGS $INCLUDES $LIBRARIES;
popd

mkdir -p geometric_shapes
pushd geometric_shapes
echo -e '\n'
echo Building geometric_shapes example
gcc -o geometric_shapes ../../examples/geometric_shapes/main.c ../../examples/geometric_shapes/game.c ../../examples/geometric_shapes/rayfork_renderer.c ../../examples/geometric_shapes/dependencies/glad/glad.c $FLAGS $INCLUDES $LIBRARIES;
popd

popd