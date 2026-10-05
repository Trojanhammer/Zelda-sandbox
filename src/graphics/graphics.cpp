#include "graphics.h"

#include <cmath>

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

void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, (int)x - 3, (int)y, (int)x + 3, (int)y);
    SDL_RenderDrawLine(renderer, (int)x, (int)y - 3, (int)x, (int)y + 3);
}

}
