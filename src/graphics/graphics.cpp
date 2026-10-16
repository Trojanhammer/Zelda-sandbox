#include "graphics.h"

#include <SDL_image.h>

#include "ui/ui.h"

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
    SDL_Rect ground = {-WINDOW_W, (int)groundY, WORLD_W + 2 * WINDOW_W, WINDOW_H - (int)groundY};
    SDL_RenderFillRect(renderer, &ground);
}

void DrawWorldMarks(SDL_Renderer* renderer, float groundY) {
    int y = (int)groundY;
    for (int x = 0; x <= WORLD_W; x += 100) {
        bool every500 = (x % 500 == 0);
        SDL_SetRenderDrawColor(renderer, every500 ? 235 : 165, every500 ? 220 : 145, every500 ? 190 : 115, 255);   // light marks on the brown ground
        SDL_Rect tick = {x - 1, y, 3, every500 ? 16 : 7};
        SDL_RenderFillRect(renderer, &tick);
        if (every500) UI::DrawText(renderer, std::to_string(x), x + 6, y + 3, 2, SDL_Color{235, 220, 190, 255});   // beside the tick, clear of the text at the bottom of the window
    }
    for (int x : {0, WORLD_W}) {                                    // a tall post at each end of the world
        SDL_SetRenderDrawColor(renderer, 60, 45, 35, 255);
        SDL_Rect post = {x - 5, y - 150, 10, 150};
        SDL_RenderFillRect(renderer, &post);
        SDL_SetRenderDrawColor(renderer, 235, 190, 60, 255);
        SDL_Rect cap = {x - 5, y - 150, 10, 10};
        SDL_RenderFillRect(renderer, &cap);
    }
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

void DrawSpriteRotated(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float angleRadians, float scale, bool flipHorizontal) {
    if (texture == nullptr) return;
    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_FRect dst = {x - w * scale / 2, y - h * scale / 2, w * scale, h * scale};   // the middle of the picture lands on (x, y)
    SDL_RenderCopyExF(renderer, texture, nullptr, &dst, angleRadians * 180.0 / 3.14159265, nullptr,
                      flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);   // SDL wants degrees
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

void DrawBoxRecallEffect(SDL_Renderer* renderer, float x, float y, float size, const std::vector<SDL_FPoint>& path) {
    float t = SDL_GetTicks() / 1000.0f;   // only used to make the glow flicker
    int s = (int)size;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 1. the path it goes back along: a bright thin line, with a soft wider one behind it (the same as the sword's)
    std::vector<SDL_FPoint> line = path;
    line.push_back({x, y});
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

    // 2. the see-through copy where it will end up (the first dot of the path): a faint golden fill and a brighter edge, both pulsing
    if (!path.empty()) {
        float pulse = 0.5f + 0.5f * std::sin(t * 6.0f);
        SDL_Rect ghost = {(int)path[0].x - s / 2, (int)path[0].y - s / 2, s, s};
        SDL_SetRenderDrawColor(renderer, 255, 205, 40, (Uint8)(35 + 30 * pulse));
        SDL_RenderFillRect(renderer, &ghost);
        SDL_SetRenderDrawColor(renderer, 255, 205, 40, (Uint8)(140 + 80 * pulse));
        SDL_RenderDrawRect(renderer, &ghost);
    }

    // 3. the frame around the box: a sharp golden one and a soft one outside it. Each flickers by itself, so it crackles a little
    //    like the sword's rim. A frame is four thin rectangles, `gap` px outside the box and `thick` px thick.
    int cx = (int)x - s / 2, cy = (int)y - s / 2;
    for (int ring = 0; ring < 2; ring++) {
        int gap = (int)((ring == 0 ? 1.0f : 4.0f) + std::sin(t * 40.0f + ring * 1.7f));
        int thick = (ring == 0) ? 2 : 3;
        SDL_SetRenderDrawColor(renderer, 255, 205, 40, (ring == 0) ? 255 : 90);
        SDL_Rect top    = {cx - gap - thick, cy - gap - thick, s + 2 * (gap + thick), thick};
        SDL_Rect bottom = {cx - gap - thick, cy + s + gap,     s + 2 * (gap + thick), thick};
        SDL_Rect left   = {cx - gap - thick, cy - gap,         thick, s + 2 * gap};
        SDL_Rect right  = {cx + s + gap,     cy - gap,         thick, s + 2 * gap};
        SDL_RenderFillRect(renderer, &top);
        SDL_RenderFillRect(renderer, &bottom);
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
    }
}

void DrawBoxStasisEffect(SDL_Renderer* renderer, float x, float y, float size, float storedVX, float storedVY,
                         float aimDegrees, float timeLeft, float flash) {
    int s = (int)size;
    int left = (int)x - s / 2, top = (int)y - s / 2;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 1. the yellow tint over the whole box, and a frame around it (a frozen box: nothing flickers, only the time bar moves)
    SDL_SetRenderDrawColor(renderer, 255, 190, 40, 85);
    SDL_Rect body = {left, top, s, s};
    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 190, 40, 235);
    for (int i = 0; i < 3; i++) {                                   // a frame 3 px thick, just outside the box
        SDL_Rect frame = {left - 1 - i, top - 1 - i, s + 2 + 2 * i, s + 2 + 2 * i};
        SDL_RenderDrawRect(renderer, &frame);
    }

    // 2. the arrow: where it will go. Its length grows with the speed the hits stored (px/s): 20 px + 0.2 per px/s, at most 280 px
    //    (for example 480 px/s = 116 px, 800 px/s = 180 px, 1200 px/s = 260 px).
    float speed = std::sqrt(storedVX * storedVX + storedVY * storedVY);
    float pi = 3.14159265f;
    float angle, length;
    bool stored = speed > 1.0f;
    if (stored) {
        angle = std::atan2(storedVY, storedVX);                      // the speed already points the way on the screen (y grows downward)
        length = std::min(280.0f, 20.0f + 0.2f * speed);
    } else {
        angle = -aimDegrees * pi / 180.0f;                           // no hit yet: the aim, as a short faint preview
        length = 46.0f;
    }
    float dx = std::cos(angle), dy = std::sin(angle);
    float startX = x + dx * (s / 2 - 6), startY = y + dy * (s / 2 - 6);   // it starts inside the edge of the box
    float tipX = x + dx * (s / 2 + length), tipY = y + dy * (s / 2 + length);
    Uint8 alpha = stored ? 255 : 130;
    float shaftLength = std::sqrt((tipX - startX) * (tipX - startX) + (tipY - startY) * (tipY - startY)) - 14.0f;   // the head is 14 px long
    if (shaftLength > 0) {
        FillTurnedRect(renderer, (startX + tipX) / 2 - dx * 7, (startY + tipY) / 2 - dy * 7, shaftLength, 9, angle, SDL_Color{150, 90, 0, alpha});   // dark edge
        FillTurnedRect(renderer, (startX + tipX) / 2 - dx * 7, (startY + tipY) / 2 - dy * 7, shaftLength, 5, angle, SDL_Color{255, 205, 40, alpha});
    }
    float baseX = tipX - dx * 14, baseY = tipY - dy * 14, px = -dy, py = dx;     // the head: a triangle, tip first
    SDL_Vertex head[3];
    head[0].position = {tipX + dx * 3, tipY + dy * 3};
    head[1].position = {baseX + px * 10, baseY + py * 10};
    head[2].position = {baseX - px * 10, baseY - py * 10};
    for (int i = 0; i < 3; i++) { head[i].color = SDL_Color{255, 205, 40, alpha}; head[i].tex_coord = {0, 0}; }
    SDL_RenderGeometry(renderer, nullptr, head, 3, nullptr, 0);

    // 3. the time left: a bar above the box that shrinks to nothing
    if (timeLeft < 0) timeLeft = 0;
    if (timeLeft > 1) timeLeft = 1;
    SDL_Rect barBack = {left, top - 18, s, 8};
    SDL_SetRenderDrawColor(renderer, 60, 45, 10, 170);
    SDL_RenderFillRect(renderer, &barBack);
    SDL_Rect barFill = {left + 1, top - 17, (int)((s - 2) * timeLeft), 6};
    SDL_SetRenderDrawColor(renderer, 255, 200, 40, 255);
    SDL_RenderFillRect(renderer, &barFill);

    // 4. a flash when it is hit: the whole box goes bright for a moment
    if (flash > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 250, 200, (Uint8)(190 * std::min(1.0f, flash)));
        SDL_RenderFillRect(renderer, &body);
    }
}

// The monster from rectangles, for when the pictures are missing. Returns the y of its highest point (the tips of the horns).
static float DrawMonsterRectangles(SDL_Renderer* renderer, float x, float y, bool facingLeft, float windup) {
    const float pi = 3.14159265f;
    float f = facingLeft ? -1.0f : 1.0f;                   // +1 looks right, -1 looks left
    int cx = (int)x, cy = (int)y;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    auto fillRect = [&](int rx, int ry, int rw, int rh, SDL_Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_Rect r = {rx, ry, rw, rh};
        SDL_RenderFillRect(renderer, &r);
    };
    const SDL_Color outline = {45, 18, 16, 255}, body = {205, 78, 52, 255}, head = {225, 98, 66, 255}, belly = {236, 172, 122, 255};
    const SDL_Color legs = {130, 46, 34, 255}, bone = {238, 228, 204, 255};

    // the club in its hand, behind the body: it points up and a little forward, and goes back over the head as the hit gets ready
    float handX = cx + f * 22.0f, handY = cy + 6.0f;
    float rest = -72.0f, raised = -125.0f;                // degrees on the screen for a monster that looks right (up is negative)
    float deg = rest + (raised - rest) * windup;
    float angle = facingLeft ? pi - deg * pi / 180.0f : deg * pi / 180.0f;
    float clubLength = 46.0f;
    FillTurnedRect(renderer, handX + std::cos(angle) * clubLength / 2, handY + std::sin(angle) * clubLength / 2, clubLength + 4, 13, angle, outline);
    FillTurnedRect(renderer, handX + std::cos(angle) * clubLength / 2, handY + std::sin(angle) * clubLength / 2, clubLength, 9, angle, SDL_Color{150, 100, 55, 255});
    FillEllipse(renderer, handX + std::cos(angle) * clubLength, handY + std::sin(angle) * clubLength, 10, 10, SDL_Color{110, 72, 40, 255});   // the heavy end

    // legs, body, belly, head (each with a dark edge)
    fillRect(cx - 17, cy + 20, 14, 18, outline);  fillRect(cx - 15, cy + 22, 10, 14, legs);
    fillRect(cx + 3, cy + 20, 14, 18, outline);   fillRect(cx + 5, cy + 22, 10, 14, legs);
    fillRect(cx - 24, cy - 6, 48, 32, outline);   fillRect(cx - 22, cy - 4, 44, 28, body);
    fillRect(cx - 12, cy + 2, 24, 17, belly);
    fillRect(cx - 22, cy - 38, 44, 38, outline);  fillRect(cx - 20, cy - 36, 40, 34, head);

    // horns, eyes (they look the way it faces), mouth with two teeth
    for (int side = -1; side <= 1; side += 2) {
        SDL_Vertex horn[3];
        horn[0].position = {(float)(cx + side * 19), (float)(cy - 36)};
        horn[1].position = {(float)(cx + side * 9), (float)(cy - 36)};
        horn[2].position = {(float)(cx + side * 17), (float)(cy - 54)};
        for (int i = 0; i < 3; i++) { horn[i].color = bone; horn[i].tex_coord = {0, 0}; }
        SDL_RenderGeometry(renderer, nullptr, horn, 3, nullptr, 0);
    }
    for (int side = -1; side <= 1; side += 2) {
        int ex = cx + side * 9 - 5;
        fillRect(ex, cy - 27, 11, 10, SDL_Color{255, 238, 150, 255});                          // the eye
        fillRect(ex + 3 + (int)(f * 3), cy - 25, 4, 6, SDL_Color{30, 10, 10, 255});             // the pupil, towards where it looks
    }
    fillRect(cx - 11, cy - 12, 22, 6, outline);
    fillRect(cx - 7, cy - 12, 4, 3, bone);
    fillRect(cx + 3, cy - 12, 4, 3, bone);

    // the hand, in front of the body
    FillEllipse(renderer, handX, handY, 7, 7, SDL_Color{236, 172, 122, 255});

    // getting ready to hit: the whole monster turns redder (a quiet warning at first, strong just before the hit)
    if (windup > 0.0f) {
        SDL_SetRenderDrawColor(renderer, 255, 40, 30, (Uint8)(150 * windup * windup));
        SDL_Rect all = {cx - 24, cy - 38, 48, 76};
        SDL_RenderFillRect(renderer, &all);
    }
    return (float)(cy - 54);
}

// The monster from its pictures (they face LEFT): the body with its feet on the ground, the club turning around the fist.
// Returns the y of the top of the picture.
static float DrawMonsterSprites(SDL_Renderer* renderer, const MonsterSprites& sprites, float x, float y, bool facingLeft, float windup, float hurt) {
    const float pi = 3.14159265f;
    float f = facingLeft ? -1.0f : 1.0f;                   // +1 looks right, -1 looks left
    int w = 0, h = 0;
    SDL_QueryTexture(sprites.body, nullptr, nullptr, &w, &h);
    float centerY = y + 36.0f - h / 2.0f;                  // the bottom of the picture sits on the ground, which is 36 px under (x, y)

    // How far the club is turned from "pointing left, flat": 55 degrees = held up and forward, 140 = lifted back behind the head.
    float turn = (55.0f + (140.0f - 55.0f) * windup) * pi / 180.0f;
    auto drawClub = [&]() {
        if (sprites.club == nullptr) return;
        // Measured on assets/bokoblin.png (87 x 96, looking left): the fist in front is at column 20, row 60, and the handle of the
        // club (assets/club.png, 65 x 24) is held 12 px in from its right end.
        const float fistX = 20.0f, fistY = 60.0f, gripInset = 12.0f, clubScale = 0.85f;
        float pivotX = facingLeft ? x - w / 2.0f + fistX : x + w / 2.0f - fistX;     // the fist, on the screen
        float pivotY = centerY - h / 2.0f + fistY;
        int cw = 0, ch = 0;
        SDL_QueryTexture(sprites.club, nullptr, nullptr, &cw, &ch);
        float half = (cw - 2.0f * gripInset) * clubScale / 2.0f;                      // from the fist to the middle of the club
        float dirX = f * std::cos(turn), dirY = -std::sin(turn);                      // from the handle to the heavy end (on the screen)
        DrawSpriteRotated(renderer, sprites.club, pivotX + dirX * half, pivotY + dirY * half, -f * turn, clubScale, !facingLeft);
    };

    // Held up it is behind the body (the fist is drawn over the handle, so it looks held). Once it is past straight up it goes in
    // front, because behind the head it would be hidden just when the player should see it.
    bool clubInFront = turn > pi / 2.0f;
    if (!clubInFront) drawClub();
    DrawSprite(renderer, sprites.body, x, centerY, 1.0f, !facingLeft);

    // getting ready to hit: the body turns redder (a quiet warning at first, strong just before the hit); the club stays brown
    if (windup > 0.0f && sprites.bodyRed != nullptr) {
        SDL_SetTextureAlphaMod(sprites.bodyRed, (Uint8)(130 * windup * windup));
        DrawSprite(renderer, sprites.bodyRed, x, centerY, 1.0f, !facingLeft);
    }
    // hit: the body flashes white and fades back
    if (hurt > 0.0f && sprites.bodyWhite != nullptr) {
        SDL_SetTextureAlphaMod(sprites.bodyWhite, (Uint8)(220 * hurt));
        DrawSprite(renderer, sprites.bodyWhite, x, centerY, 1.0f, !facingLeft);
    }
    if (clubInFront) drawClub();
    return centerY - h / 2.0f;
}

void DrawMonster(SDL_Renderer* renderer, const MonsterSprites& sprites, float x, float y, bool facingLeft, float health, float windup, float hurt) {
    windup = std::clamp(windup, 0.0f, 1.0f);
    hurt = std::clamp(hurt, 0.0f, 1.0f);
    health = std::clamp(health, 0.0f, 1.0f);
    float top = sprites.body != nullptr ? DrawMonsterSprites(renderer, sprites, x, y, facingLeft, windup, hurt)
                                        : DrawMonsterRectangles(renderer, x, y, facingLeft, windup);

    // the health bar above its head
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 30, 20, 20, 220);
    SDL_Rect back = {(int)x - 28, (int)top - 16, 56, 8};
    SDL_RenderFillRect(renderer, &back);
    SDL_SetRenderDrawColor(renderer, (Uint8)(230 - 150 * health), (Uint8)(60 + 160 * health), 50, 255);
    SDL_Rect fill = {(int)x - 27, (int)top - 15, (int)(54 * health), 6};
    SDL_RenderFillRect(renderer, &fill);
}

// A disc made of square blocks (cell px wide) around (cx, cy), so that it is as chunky as the pixel art.
static void FillBlockyDisc(SDL_Renderer* renderer, float cx, float cy, float radius, SDL_Color color, int cell) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int n = (int)(radius / cell) + 1;
    for (int j = -n; j < n; j++) {
        for (int i = -n; i < n; i++) {
            float mx = (i + 0.5f) * cell, my = (j + 0.5f) * cell;   // the middle of this block, measured from the middle of the disc
            if (mx * mx + my * my > radius * radius) continue;
            SDL_FRect block = {cx + i * cell, cy + j * cell, (float)cell, (float)cell};
            SDL_RenderFillRectF(renderer, &block);
        }
    }
}

static void DrawSun(SDL_Renderer* renderer, float x, float y) {
    const int cell = 4;
    float pulse = 1.0f + 0.05f * std::sin(SDL_GetTicks() / 1000.0f * 1.2f);   // the glow breathes slowly
    // The glow ADDS light (three discs, bigger and fainter), so the clouds that slide past behind it still show through it.
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    FillBlockyDisc(renderer, x, y, 78.0f * pulse, SDL_Color{255, 190, 80, 12}, cell);
    FillBlockyDisc(renderer, x, y, 60.0f * pulse, SDL_Color{255, 190, 80, 18}, cell);
    FillBlockyDisc(renderer, x, y, 46.0f * pulse, SDL_Color{255, 200, 90, 28}, cell);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    FillBlockyDisc(renderer, x, y, 30.0f, SDL_Color{255, 206, 92, 255}, cell);   // the sun
    FillBlockyDisc(renderer, x, y, 22.0f, SDL_Color{255, 226, 132, 255}, cell);  // its brighter middle
}

void DrawBackground(SDL_Renderer* renderer, SDL_Texture* sky, SDL_Texture* castle, float cameraX) {
    const float px = 3.0f;           // one picture pixel is 3 screen pixels
    float slide = cameraX * 0.08f;   // the sky moves 8 % of what the camera moves: the farther away, the slower
    int w = 0, h = 0;
    if (sky != nullptr) {
        SDL_QueryTexture(sky, nullptr, nullptr, &w, &h);
        float shown = std::min(slide, w * px - WINDOW_W);   // never slide past the edge of the picture
        SDL_FRect dst = {-shown, WINDOW_H - h * px, w * px, h * px};
        SDL_RenderCopyF(renderer, sky, nullptr, &dst);
    }
    DrawSun(renderer, 880.0f, 76.0f);
    if (castle != nullptr) {
        SDL_QueryTexture(castle, nullptr, nullptr, &w, &h);
        // on the hills (its bottom is at y = 444, 18 px above the ground), 840 px from the left of the window when the camera is at 0
        SDL_FRect dst = {840.0f - slide - w * px / 2, 444.0f - h * px, w * px, h * px};
        SDL_SetTextureColorMod(castle, 190, 208, 232);   // a little blue haze: it is far away
        SDL_SetTextureAlphaMod(castle, 235);
        SDL_RenderCopyF(renderer, castle, nullptr, &dst);
    }
}

void DrawHearts(SDL_Renderer* renderer, int x, int y, float health, float maxHealth) {
    // one heart is 7 x 6 "pixels", each pixel 3 x 3 on the screen
    static const char* shape[6] = {".XX.XX.", "XXXXXXX", "XXXXXXX", ".XXXXX.", "..XXX..", "...X..."};
    const int px = 3, hearts = (int)std::lround(maxHealth / 20.0f);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int h = 0; h < hearts; h++) {
        float share = (health - 20.0f * h) / 20.0f;            // how much of this heart is left: 0 to 1
        if (share < 0) share = 0;
        if (share > 1) share = 1;
        int left = x + h * (7 * px + 5);
        for (int row = 0; row < 6; row++) {
            for (int col = 0; col < 7; col++) {
                if (shape[row][col] != 'X') continue;
                bool full = (share >= 1.0f) || (share >= 0.5f && col < 4);            // a half heart is red on its left side only
                SDL_Color c = full ? SDL_Color{225, 40, 50, 255} : SDL_Color{110, 100, 105, 200};
                if (full && row == 1 && col == 1) c = SDL_Color{255, 160, 165, 255};   // a little shine
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                SDL_Rect r = {left + col * px, y + row * px, px, px};
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }
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

void DrawBox(SDL_Renderer* renderer, float x, float y, float size, bool held) {
    float t = SDL_GetTicks() / 1000.0f;                       // only used to make the glow pulse
    float pulse = 0.5f + 0.5f * std::sin(t * 6.0f);           // goes between 0 and 1, about once a second
    int s = (int)size;
    int left = (int)x - s / 2, top = (int)y - s / 2;
    auto fillRect = [&](int rx, int ry, int rw, int rh, SDL_Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_Rect r = {rx, ry, rw, rh};
        SDL_RenderFillRect(renderer, &r);
    };

    // 1. the glow behind the box: three red rings that grow and fade outwards, and a thin blue rim close to the box
    if (held) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int ring = 3; ring >= 1; ring--) {
            int grow = ring * 4 + (int)(pulse * 4);
            fillRect(left - grow, top - grow, s + 2 * grow, s + 2 * grow, SDL_Color{255, 70, 60, (Uint8)((90 - ring * 22) * (0.6f + 0.4f * pulse))});
        }
        SDL_SetRenderDrawColor(renderer, 80, 140, 255, (Uint8)(120 + 100 * pulse));
        SDL_Rect rim = {left - 2, top - 2, s + 4, s + 4};
        SDL_RenderDrawRect(renderer, &rim);
    }

    // 2. the body: a dark outline, the steel, a lit edge on the top and left and a shaded one on the bottom and right
    const SDL_Color steel = {120, 134, 150, 255}, lit = {178, 192, 208, 255}, shade = {72, 84, 98, 255}, dark = {32, 38, 48, 255};
    fillRect(left, top, s, s, dark);
    fillRect(left + 3, top + 3, s - 6, s - 6, steel);
    fillRect(left + 3, top + 3, s - 6, 4, lit);
    fillRect(left + 3, top + 3, 4, s - 6, lit);
    fillRect(left + 3, top + s - 7, s - 6, 4, shade);
    fillRect(left + s - 7, top + 3, 4, s - 6, shade);

    // 3. the panel in the middle with two braces across it (the braces stick out a little, so the panel's edge is drawn over them)
    int pad = 14, panel = s - 2 * pad;
    fillRect(left + pad, top + pad, panel, panel, SDL_Color{100, 113, 128, 255});
    float middleX = left + s / 2.0f, middleY = top + s / 2.0f, brace = (panel - 4) * 1.4142f;
    FillTurnedRect(renderer, middleX, middleY, brace, 5, 0.7854f, shade);      // 0.7854 radians = 45 degrees
    FillTurnedRect(renderer, middleX, middleY, brace, 5, -0.7854f, shade);
    fillRect(left + pad, top + pad, panel, 2, shade);                          // the panel's four edges, on top of the braces
    fillRect(left + pad, top + pad + panel - 2, panel, 2, shade);
    fillRect(left + pad, top + pad, 2, panel, shade);
    fillRect(left + pad + panel - 2, top + pad, 2, panel, shade);

    // 4. a rivet in each corner: a small shadow, then the bright head
    const int rivetAt[2] = {7, s - 14};
    for (int cx = 0; cx < 2; cx++) {
        for (int cy = 0; cy < 2; cy++) {
            fillRect(left + rivetAt[cx] + 1, top + rivetAt[cy] + 1, 6, 6, dark);
            fillRect(left + rivetAt[cx], top + rivetAt[cy], 5, 5, SDL_Color{205, 216, 228, 255});
        }
    }

    // 5. while it is held the whole box gets a very faint red tint that pulses with the glow (more would hide the steel)
    if (held) {
        fillRect(left, top, s, s, SDL_Color{255, 70, 60, (Uint8)(10 + 22 * pulse)});
    }
}

void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, (int)x - 3, (int)y, (int)x + 3, (int)y);
    SDL_RenderDrawLine(renderer, (int)x, (int)y - 3, (int)x, (int)y + 3);
}

}
