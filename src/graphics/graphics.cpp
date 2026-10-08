#include "graphics.h"

#include <SDL_image.h>

#include <algorithm>
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

    // A filled ellipse made of a fan of triangles (SDL has no ellipse). A circle when rx == ry.
    void FillEllipse(SDL_Renderer* renderer, float cx, float cy, float rx, float ry, SDL_Color color) {
        const int segments = 28;
        SDL_Vertex v[segments + 2];
        v[0].position = {cx, cy};
        v[0].color = color;
        v[0].tex_coord = {0, 0};
        for (int i = 0; i <= segments; i++) {
            float a = i * 2.0f * 3.14159265f / segments;
            v[i + 1].position = {cx + rx * std::cos(a), cy + ry * std::sin(a)};
            v[i + 1].color = color;
            v[i + 1].tex_coord = {0, 0};
        }
        int indices[segments * 3];
        for (int i = 0; i < segments; i++) {
            indices[i * 3] = 0;
            indices[i * 3 + 1] = i + 1;
            indices[i * 3 + 2] = i + 2;
        }
        SDL_RenderGeometry(renderer, nullptr, v, segments + 2, indices, segments * 3);
    }

    // One tongue of flame standing on the ground at baseX: wide at the bottom, a tip at the top. `sway` pushes the tip sideways.
    void FillFlameTongue(SDL_Renderer* renderer, float baseX, float groundY, float halfWidth, float height, float sway, SDL_Color color) {
        SDL_FPoint p[6] = {
            {baseX, groundY - height * 0.15f},                              // 0 the middle of the base (the fan starts here)
            {baseX - halfWidth, groundY},                                   // 1 bottom left
            {baseX - halfWidth * 0.85f + sway * 0.4f, groundY - height * 0.45f},   // 2 left side
            {baseX + sway, groundY - height},                               // 3 the tip
            {baseX + halfWidth * 0.85f + sway * 0.4f, groundY - height * 0.45f},   // 4 right side
            {baseX + halfWidth, groundY},                                   // 5 bottom right
        };
        SDL_Vertex v[6];
        for (int i = 0; i < 6; i++) {
            v[i].position = p[i];
            v[i].color = color;
            v[i].tex_coord = {0, 0};
        }
        const int indices[12] = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5};
        SDL_RenderGeometry(renderer, nullptr, v, 6, indices, 12);
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

SDL_Texture* LoadSilhouette(SDL_Renderer* renderer, const char* path, SDL_Color color) {
    SDL_Surface* loaded = IMG_Load(path);
    if (loaded == nullptr) {
        std::fprintf(stderr, "could not load %s: %s\n", path, IMG_GetError());
        return nullptr;
    }
    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_RGBA32, 0);   // bytes in memory: R, G, B, A
    SDL_FreeSurface(loaded);
    if (rgba == nullptr) return nullptr;
    for (int y = 0; y < rgba->h; y++) {
        for (int x = 0; x < rgba->w; x++) {
            Uint8* pixel = (Uint8*)rgba->pixels + y * rgba->pitch + x * 4;
            pixel[0] = color.r;   // paint it, but leave pixel[3] (the alpha) alone, so the shape stays
            pixel[1] = color.g;
            pixel[2] = color.b;
        }
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, rgba);
    SDL_FreeSurface(rgba);
    if (texture != nullptr) SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

void DrawRecallEffect(SDL_Renderer* renderer, SDL_Texture* silhouette, float swordX, float swordY, float swordAngle, const std::vector<SDL_FPoint>& path) {
    if (silhouette == nullptr) return;
    const float pi = 3.14159265f;
    float t = SDL_GetTicks() / 1000.0f;   // only used to make the glow flicker

    // 1. the path it goes back along: a bright thin line, with a soft wider one behind it
    std::vector<SDL_FPoint> line = path;
    line.push_back({swordX, swordY});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (size_t i = 0; i + 1 < line.size(); i++) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 40, 70);
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx != 0 || dy != 0) SDL_RenderDrawLineF(renderer, line[i].x + dx, line[i].y + dy, line[i + 1].x + dx, line[i + 1].y + dy);
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 190, 20, 235);
        SDL_RenderDrawLineF(renderer, line[i].x, line[i].y, line[i + 1].x, line[i + 1].y);
    }

    // 2. the see-through copy where it will end up (the first dot), pointing the way it was thrown
    if (!path.empty()) {
        float angle = 0.0f;
        if (path.size() >= 2) angle = std::atan2(path[1].y - path[0].y, path[1].x - path[0].x);
        float pulse = 0.5f + 0.5f * std::sin(t * 6.0f);
        SDL_SetTextureAlphaMod(silhouette, (Uint8)(90 + 60 * pulse));
        DrawSpriteRotated(renderer, silhouette, path[0].x, path[0].y, angle);
    }

    // 3. the rim: copies of the silhouette a few pixels around the sword (a sharp ring and a soft one). Each copy flickers
    //    by itself, so the rim crackles a little like the one in the game.
    for (int ring = 0; ring < 2; ring++) {
        float radius = (ring == 0) ? 2.0f : 4.5f;
        SDL_SetTextureAlphaMod(silhouette, (ring == 0) ? 255 : 80);
        for (int k = 0; k < 8; k++) {
            float a = k * pi / 4;
            float r = radius + std::sin(t * 40.0f + k * 1.7f + ring);
            DrawSpriteRotated(renderer, silhouette, swordX + r * std::cos(a), swordY + r * std::sin(a), swordAngle);
        }
    }
    SDL_SetTextureAlphaMod(silhouette, 255);
}

void DrawFire(SDL_Renderer* renderer, SDL_Texture* flame, float x, float groundY, float radius, float intensity) {
    intensity = std::clamp(intensity, 0.0f, 1.0f);
    float t = SDL_GetTicks() / 1000.0f;                 // only used to make the flame flicker
    float scale = 0.25f + 0.75f * intensity;            // a weak fire is a quarter of the size, a strong one fills the circle
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // the ring on the grass: how far the updraft reaches (a flat ellipse, drawn as a circle seen from the side)
    FillEllipse(renderer, x, groundY, radius, radius * 0.16f, SDL_Color{60, 40, 30, (Uint8)(60 + 60 * intensity)});

    float flicker = 1.0f + 0.05f * std::sin(t * 17.0f) + 0.03f * std::sin(t * 29.0f);
    if (flame != nullptr) {
        int w = 0, h = 0;
        SDL_QueryTexture(flame, nullptr, nullptr, &w, &h);
        float width = 2.0f * radius * scale;
        float height = width * h / w * flicker;          // the picture keeps its shape; the height wobbles a little
        SDL_FRect dst = {x - width / 2, groundY - height, width, height};   // the bottom edge of the picture is on the ground
        SDL_RenderCopyF(renderer, flame, nullptr, &dst);
        return;
    }

    // no picture: three tongues, each in three layers (red outside, orange, a yellow core)
    const SDL_Color layerColor[3] = {{225, 60, 30, 235}, {255, 150, 30, 245}, {255, 232, 110, 255}};
    const float layerWidth[3] = {1.0f, 0.68f, 0.38f};
    const float layerHeight[3] = {1.0f, 0.78f, 0.52f};
    const float tongueOffset[3] = {-0.58f, 0.0f, 0.58f};     // left, middle, right
    const float tongueHeight[3] = {0.62f, 1.0f, 0.7f};
    for (int layer = 0; layer < 3; layer++) {
        for (int k = 0; k < 3; k++) {
            float halfWidth = radius * scale * 0.5f * layerWidth[layer];
            float height = radius * 2.2f * scale * tongueHeight[k] * layerHeight[layer] * flicker;
            float sway = radius * scale * 0.18f * std::sin(t * (9.0f + 3 * k) + k * 2.1f);
            FillFlameTongue(renderer, x + tongueOffset[k] * radius * scale * 0.9f, groundY, halfWidth, height, sway, layerColor[layer]);
        }
    }
}

void DrawUpdraftColumn(SDL_Renderer* renderer, float x, float groundY, float radius, float columnHeight, float intensity) {
    intensity = std::clamp(intensity, 0.0f, 1.0f);
    if (intensity <= 0.0f) return;
    float t = SDL_GetTicks() / 1000.0f;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // the zone the updraft fills: a very faint box
    SDL_SetRenderDrawColor(renderer, 255, 190, 110, (Uint8)(28 * intensity));
    SDL_FRect zone = {x - radius, groundY - columnHeight, 2.0f * radius, columnHeight};
    SDL_RenderFillRectF(renderer, &zone);

    // streaks of rising air: each one has its own starting height, moves up and fades near the top
    const int streaks = 12;
    float speed = 60.0f + 160.0f * intensity;            // px per second: stronger air moves faster
    for (int i = 0; i < streaks; i++) {
        float sx = x - radius + (i + 0.5f) * (2.0f * radius / streaks);
        float phase = std::fmod(i * 0.61803f, 1.0f);      // spreads the starting heights without using random numbers
        float up = std::fmod(t * speed + phase * columnHeight, columnHeight);
        float fade = 1.0f - up / columnHeight;
        float length = 12.0f + 16.0f * intensity;
        SDL_SetRenderDrawColor(renderer, 255, 215, 150, (Uint8)(170 * intensity * fade));
        SDL_FRect streak = {sx - 1.0f, groundY - up - length, 2.0f, length};
        SDL_RenderFillRectF(renderer, &streak);
    }
}

void DrawFireball(SDL_Renderer* renderer, SDL_Texture* fireball, float x, float y, float angle) {
    if (fireball != nullptr) {
        DrawSpriteRotated(renderer, fireball, x, y, angle);
        return;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    float c = std::cos(angle), s = std::sin(angle);
    for (int i = 4; i >= 1; i--) {                       // the tail: smaller and fainter circles behind the ball
        float back = i * 7.0f;
        FillEllipse(renderer, x - c * back, y - s * back, 9.0f - i * 1.5f, 9.0f - i * 1.5f, SDL_Color{240, 100, 30, (Uint8)(210 - i * 40)});
    }
    FillEllipse(renderer, x, y, 10.0f, 10.0f, SDL_Color{255, 150, 30, 255});     // the ball
    FillEllipse(renderer, x + c * 1.5f, y + s * 1.5f, 5.5f, 5.5f, SDL_Color{255, 235, 120, 255});   // its hot core
}

void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, (int)x - 3, (int)y, (int)x + 3, (int)y);
    SDL_RenderDrawLine(renderer, (int)x, (int)y - 3, (int)x, (int)y + 3);
}

}
