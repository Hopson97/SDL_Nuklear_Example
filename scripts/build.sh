#!/bin/bash

target_release() {
    cd release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
    echo "Built target in bin/release/"
    cd ../..
}

target_debug() {
    cd debug 
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make
    echo "Built target in bin/debug/"
    cd ../..
}

# Create folder for distribution
if [ "$1" = "release" ]
then
    if [ -d "$nukclear_sdl_gl_test " ]
    then
        rm -rf -d nukclear_sdl_gl_test 
    fi

    mkdir -p nukclear_sdl_gl_test 
fi

# Creates the folder for the binaries
mkdir -p nukclear_sdl_gl_test 
mkdir -p bin
cd bin 
mkdir -p release
mkdir -p debug

# Builds target
if [ "$1" = "release" ]
then
    target_release
    cp bin/release/nukclear_sdl_gl_test  nukclear_sdl_gl_test /nukclear_sdl_gl_test 
else
    target_debug
fi

cp -R Data nukclear_sdl_gl_test /Data
