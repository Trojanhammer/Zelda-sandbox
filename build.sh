#!/bin/bash
# Builds ./sandbox from everything under src/. Run from this folder:  ./build.sh && ./sandbox
cd "$(dirname "$0")"
clang++ -std=c++17 -g -O0 -Wall $(find src -name '*.cpp') -Isrc -Isrc/ui -Isrc/graphics -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE \
  -L/opt/homebrew/lib -lSDL2main -lSDL2 -Wl,-framework,Cocoa -o sandbox
