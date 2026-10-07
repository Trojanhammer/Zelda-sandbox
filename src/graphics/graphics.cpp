#include "graphics.h"

#include <SDL_image.h>

#include <cmath>
#include <cstdio>

namespace {
    // Fills a rectangle that is turned by `angle` around its own middle. SDL's normal rectangles cannot turn,
    // so the four corners are worked out here and drawn as two triangles.
    void FillTurnedRect(SDL_Renderer* renderer, float centerX, float centerY, float length, float width, float angle, SDL_Color color) {
        float c = std::cos(angle), s = std::sin(angle);
        float halfL = length / 2, halfW = width / 2;
        // the four corners in the rectangle's own frame (along, across), turned by the angle and moved to the centre
        const float corners[4][2] = {{-halfL, -halfW}, {halfL, -halfW}, {halfL, halfW}, {-halfL, halfW}};
        SDL_Vertex v[4];
        for (int i = 0; i < 4; i++) {
            v[i].position.x = centerX + corners[i][0] * c - corners[i][1] * s;
            v[i].position.y = centerY + corners[i][0] * s + corners[i][1] * c;
            v[i].color = color;
            v[i].tex_coord = {0, 0};
        }
        const int indices[6] = {0, 1, 2, 0, 2, 3};
        SDL_RenderGeometry(renderer, nullptr, v, 4, indices, 6);
    }
}

namespace Gfx {

void DrawGround(SDL_Renderer* renderer, float groundY) {
    SDL_SetRenderDrawColor(renderer, 120, 100, 80, 255);
    SDL_Rect ground = {0, (int)groundY, WINDOW_W, WINDOW_H - (int)groundY};
    SDL_RenderFillRect(renderer, &ground);
}

void DrawHand(SDL_Renderer* renderer, float x, float y) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
    SDL_Rect hand = {(int)x - 6, (int)y - 6, 12, 12};
    SDL_RenderFillRect(renderer, &hand);
}

void DrawSword(SDL_Renderer* renderer, float x, float y, float angle) {
    // 52 px long in all: a 40 px blade at the front and a 12 px handle at the back, with a short bar between (the guard)
    float c = std::cos(angle), s = std::sin(angle);
    FillTurnedRect(renderer, x + 6 * c, y + 6 * s, 40, 6, angle, SDL_Color{170, 180, 195, 255});     // blade (centre 6 px ahead of the middle)
    FillTurnedRect(renderer, x - 20 * c, y - 20 * s, 12, 5, angle, SDL_Color{110, 70, 40, 255});     // handle
    FillTurnedRect(renderer, x - 14 * c, y - 14 * s, 3, 14, angle, SDL_Color{200, 170, 60, 255});    // guard, across the sword
}

void DrawAimLine(SDL_Renderer* renderer, float x, float y, float angleDegrees) {
    float a = angleDegrees * 3.14159265f / 180.0f;
    float dx = std::cos(a), dy = -std::sin(a);      // up on the screen is a smaller y, so the y part is flipped
    SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255);
    for (int i = 1; i <= 5; i++) {                  // five dots, 18 px apart, starting 18 px from the hand
        float px = x + dx * 18.0f * i, py = y + dy * 18.0f * i;
        int size = (i == 5) ? 7 : 4;                // the last one is bigger: the "tip"
        SDL_Rect dot = {(int)px - size / 2, (int)py - size / 2, size, size};
        SDL_RenderFillRect(renderer, &dot);
    }
}

SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");   // "nearest": pixel art stays sharp when it is scaled or turned
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (texture == nullptr) std::fprintf(stderr, "could not load %s: %s\n", path, IMG_GetError());
    return texture;
}

void DrawSpriteRotated(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float angleRadians, float scale) {
    if (texture == nullptr) return;
    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_FRect dst = {x - w * scale / 2, y - h * scale / 2, w * scale, h * scale};   // the middle of the picture lands on (x, y)
    SDL_RenderCopyExF(renderer, texture, nullptr, &dst, angleRadians * 180.0 / 3.14159265, nullptr, SDL_FLIP_NONE);   // SDL wants degrees
}

void DrawSprite(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float scale, bool flipHorizontal) {
    if (texture == nullptr) return;
    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_FRect dst = {x - w * scale / 2, y - h * scale / 2, w * scale, h * scale};
    SDL_RenderCopyExF(renderer, texture, nullptr, &dst, 0.0, nullptr, flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void DrawWalker(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, bool walking, bool flipHorizontal, float scale) {
    if (texture == nullptr) return;
    if (!walking) {
        DrawSprite(renderer, texture, x, y, scale, flipHorizontal);
        return;
    }
    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

    // Measured on assets/link.png (55 x 112), in picture pixels:
    const int hipRow = 73;                 // from this row down it is legs, above it is the body (the tunic ends at row 72)
    const int overlap = 2;                 // the legs start 2 rows higher and hide behind the tunic, so no gap shows when they turn
    const int cutColumn = 28;              // back leg (left in the picture): columns 0..27, front leg: 28..54
    const float hipBackX = 24, hipFrontX = 32;   // the point each leg swings around
    const float strideLength = 80.0f;      // one full step (both legs) per 80 px walked: 150 px/s is about 2 steps a second
    const float maxSwing = 22.0f;          // degrees
    const float legLength = 38.0f;         // hip row 73 down to the bottom of the boot, row 111

    float phase = (flipHorizontal ? -x : x) / strideLength * 2.0f * 3.14159265f;   // grows while he walks the way he faces
    float swing = maxSwing * std::sin(phase);
    float angleDegrees = std::fabs(swing);
    // A turned leg is shorter on the screen (38 px * cos 22 degrees = 35 px), so the body goes down to keep the feet on the ground
    float drop = legLength * (1.0f - std::cos(angleDegrees * 3.14159265f / 180.0f));

    float left = x - w * scale / 2;
    float top = y - h * scale / 2 + drop * scale;

    struct Leg { int srcX, srcW; float hipX; float angle; };
    Leg legs[2] = {
        {0, cutColumn, hipBackX, swing},                   // back leg swings one way ...
        {cutColumn, w - cutColumn, hipFrontX, -swing},     // ... the front leg the other way
    };
    int legTop = hipRow - overlap;
    for (const Leg& leg : legs) {
        SDL_Rect src = {leg.srcX, legTop, leg.srcW, h - legTop};
        float dstX = flipHorizontal ? left + (w - leg.srcX - leg.srcW) * scale : left + leg.srcX * scale;
        SDL_FRect dst = {dstX, top + legTop * scale, leg.srcW * scale, (h - legTop) * scale};
        SDL_FPoint hip = {(flipHorizontal ? leg.srcX + leg.srcW - leg.hipX : leg.hipX - leg.srcX) * scale, (hipRow - legTop) * scale};
        double angle = flipHorizontal ? -leg.angle : leg.angle;
        SDL_RenderCopyExF(renderer, texture, &src, &dst, angle, &hip, flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
    // the body goes on top of the legs
    SDL_Rect bodySrc = {0, 0, w, hipRow};
    SDL_FRect bodyDst = {left, top, w * scale, hipRow * scale};
    SDL_RenderCopyExF(renderer, texture, &bodySrc, &bodyDst, 0.0, nullptr, flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, (int)x - 3, (int)y, (int)x + 3, (int)y);
    SDL_RenderDrawLine(renderer, (int)x, (int)y - 3, (int)x, (int)y + 3);
}

}
