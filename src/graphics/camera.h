#pragma once
#include <SDL.h>

// The camera: the world is wider than the window (Gfx::WORLD_W px), and the window shows 960 px of it. Everything is still
// drawn with its world position (a box at x = 3000 is drawn at x = 3000); the camera just moves the window along the world.
//     camera.Follow(Link's x, deltaTime);   // once a frame: where should the window be?
//     camera.Begin(renderer);               // everything drawn from here on is in world positions
//     ... DrawGround, Link, the box, the sword ...
//     camera.End(renderer);                 // back to screen positions: the text on the screen does not move
class Camera {
    public:
        float x = 0.0f;   // the world x of the left edge of the window (0 = the start of the world)

    public:
        // Moves the window smoothly so that targetX is in the middle of it (frame rate does not matter), and never shows
        // anything outside the world: the window stops at 0 and at WORLD_W - 960.
        void Follow(float targetX, float deltaTime);
        void Begin(SDL_Renderer* renderer);
        void End(SDL_Renderer* renderer);
};
