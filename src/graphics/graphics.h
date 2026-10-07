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

    // A dotted line from (x, y) in the direction of angleDegrees (0 = right, 90 = up, 180 = left, the same as Throw()),
    // with a bigger dot at the end. Shows where a throw would go.
    void DrawAimLine(SDL_Renderer* renderer, float x, float y, float angleDegrees);

    // Loads a picture (PNG) as a texture. Returns nullptr, and prints why on stderr, if the file is missing, so the game can
    // still run and fall back to the rectangle drawings above. Free it with SDL_DestroyTexture before the renderer goes.
    SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* path);

    // Draws a picture with its MIDDLE at (x, y), `scale` times its own size (2.0 = twice as big). flipHorizontal = mirror it
    // (Link faces right in the picture, so mirror it to make him face left). Does nothing if texture is nullptr.
    void DrawSprite(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float scale = 1.0f, bool flipHorizontal = false);

    // Same, turned by angleRadians around its middle: 0 = as in the picture, positive turns clockwise on the screen.
    // The sword picture points RIGHT, so atan2(speedY, speedX) makes it point the way it flies, like DrawSword does.
    void DrawSpriteRotated(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float angleRadians, float scale = 1.0f);

    // Link with moving legs. walking = false draws him standing (the picture as it is); walking = true swings his two legs
    // while the body bobs. The swing follows how far he has walked (x), so his feet do not slide on the ground:
    // one full step (both legs) per 80 px. flipHorizontal = face left (the legs then swing as if walking forward to the left).
    // The numbers inside are measured on assets/link.png (55 x 112): legs start at row 73, the two legs are cut apart at column 28.
    void DrawWalker(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, bool walking, bool flipHorizontal = false, float scale = 1.0f);

    // A small cross with its middle at (x, y). Handy to look at the recorded dots of a path.
    void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color);
}
