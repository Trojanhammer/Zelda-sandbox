#include "camera.h"
#include "graphics.h"

#include <cmath>

void Camera::Follow(float targetX, float deltaTime) {
    float wanted = targetX - Gfx::WINDOW_W / 2.0f;              // the left edge that puts the target in the middle
    float farthest = (float)(Gfx::WORLD_W - Gfx::WINDOW_W);     // the window must not go past the end of the world
    if (wanted < 0.0f) wanted = 0.0f;
    if (wanted > farthest) wanted = farthest;
    // Go a fraction of the way there each frame. The fraction depends on deltaTime, so it looks the same at 30 and at 144 fps:
    // about 0.13 s to cover the first half of the distance (the rate 8 means "63 % of the way in 1/8 s").
    x += (wanted - x) * (1.0f - std::exp(-deltaTime * 8.0f));
}

void Camera::Begin(SDL_Renderer* renderer) {
    // The viewport is the part of the window we draw into, and (0, 0) of everything drawn afterwards is ITS top-left corner.
    // Moving its left edge to -x moves the whole world x px to the left. It is made wider by x so its right edge stays at the
    // window's right edge.
    int shift = (int)std::lround(x);
    SDL_Rect view = {-shift, 0, Gfx::WINDOW_W + shift, Gfx::WINDOW_H};
    SDL_RenderSetViewport(renderer, &view);
}

void Camera::End(SDL_Renderer* renderer) {
    SDL_RenderSetViewport(renderer, nullptr);   // the whole window again
}
