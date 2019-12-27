#!/bin/bash

INCLUDES="-I../../dependencies/cgltf -I../../dependencies/dr_libs -I../../dependencies/jar -I../../dependencies/miniaudio -I../../dependencies/par -I../../dependencies/stb -I../../dependencies/tinyobjloader-c -I../../examples/dependencies/ -I../../examples/dependencies/sokol -I../../rayfork"

if [ "$(uname)" = "Darwin" ]
then
  FLAGS="-g -D_DEFAULT_SOURCE -ObjC -fobjc-arc"
  LIBRARIES="-framework Cocoa"
else
  FLAGS="-g -D_DEFAULT_SOURCE"
  LIBRARIES="-lGL -lm -lpthread -ldl -lrt -lX11"
fi

mkdir -p bin
pushd bin

mkdir -p basic_shapes
pushd basic_shapes
echo -e '\n'
echo Building basic_shapes example
gcc -o basic_shapes ../../examples/basic_shapes/main.c ../../examples/basic_shapes/game.c ../../examples/dependencies/glad/glad.c $FLAGS $INCLUDES $LIBRARIES;
popd

mkdir -p collision_area
pushd collision_area
echo -e '\n'
echo Building collision_area example
gcc -o collision_area ../../examples/collision_area/main.c ../../examples/collision_area/game.c ../../examples/dependencies/glad/glad.c $FLAGS $INCLUDES $LIBRARIES;
popd

mkdir -p geometric_shapes
pushd geometric_shapes
echo -e '\n'
echo Building geometric_shapes example
gcc -o geometric_shapes ../../examples/geometric_shapes/main.c ../../examples/geometric_shapes/game.c ../../examples/dependencies/glad/glad.c $FLAGS $INCLUDES $LIBRARIES;
popd

popd
