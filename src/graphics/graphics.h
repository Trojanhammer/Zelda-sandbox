#pragma once
#include <SDL.h>

// Everything that only draws. None of it moves or decides anything: you give it a position and it paints.
// Coordinates are pixels in the window (960 x 540): x grows to the right, y grows DOWNWARDS, (0,0) is the top-left corner.
namespace Gfx {
    const int WINDOW_W = 960;
    const int WINDOW_H = 540;

    // The ground: a brown block from groundY down to the bottom of the window.
    void DrawGround(SDL_Renderer* renderer, float groundY);

    // A small dark square with its middle at (x, y). Stands for the hand the sword is thrown from.
    void DrawHand(SDL_Renderer* renderer, float x, float y);

    // A sword with its middle at (x, y). angle is in radians: 0 = pointing right, 1.5708 (a quarter turn) = pointing down,
    // -1.5708 = pointing up. For a flying sword use atan2(speedY, speedX) so that it points the way it moves.
    void DrawSword(SDL_Renderer* renderer, float x, float y, float angle);

    // A small cross with its middle at (x, y). Handy to look at the recorded dots of a path.
    void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color);
}
