#!/bin/sh
export CXX="g++"
export CXXFLAGS="-std=c++1y -Wall -DGLM_FORCE_RADIANS -DGLEW_STATIC"
export LDFLAGS="-static"
export LDLIBS="-lglfw3 -lglew32 -lpng -lz -lopengl32 -lgdi32"
respite
