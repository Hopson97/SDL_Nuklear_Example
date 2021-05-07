#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/nukclear_sdl_gl_test 
else
    ./bin/debug/nukclear_sdl_gl_test 
fi