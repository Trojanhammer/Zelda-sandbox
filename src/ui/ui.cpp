#include "ui.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>

namespace {
    const int GLYPH_W = 5;
    const int GLYPH_H = 7;
    const int ADVANCE = GLYPH_W + 1;   // one square of gap after each letter

    struct Glyph {
        char letter;
        uint8_t rows[GLYPH_H];   // top row first; in each row the lowest 5 bits are the squares, left square = bit 4
    };

    // 0b01110 means: gap, square, square, square, gap.
    const Glyph FONT[] = {
        {'A', {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
        {'B', {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110}},
        {'C', {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110}},
        {'D', {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110}},
        {'E', {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111}},
        {'F', {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000}},
        {'G', {0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111}},
        {'H', {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
        {'I', {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}},
        {'J', {0b00111, 0b00010, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100}},
        {'K', {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001}},
        {'L', {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111}},
        {'M', {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001}},
        {'N', {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001}},
        {'O', {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}},
        {'P', {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000}},
        {'Q', {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101}},
        {'R', {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001}},
        {'S', {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110}},
        {'T', {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100}},
        {'U', {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}},
        {'V', {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100}},
        {'W', {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010}},
        {'X', {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001}},
        {'Y', {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100}},
        {'Z', {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111}},
        {'0', {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110}},
        {'1', {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}},
        {'2', {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111}},
        {'3', {0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110}},
        {'4', {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010}},
        {'5', {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110}},
        {'6', {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110}},
        {'7', {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000}},
        {'8', {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110}},
        {'9', {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100}},
        {' ', {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}},
        {'.', {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01100, 0b01100}},
        {',', {0b00000, 0b00000, 0b00000, 0b00000, 0b01100, 0b00100, 0b01000}},
        {'!', {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000, 0b00100}},
        {'?', {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b00000, 0b00100}},
        {':', {0b00000, 0b01100, 0b01100, 0b00000, 0b01100, 0b01100, 0b00000}},
        {'-', {0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000}},
        {'/', {0b00001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000}},
        {'\'', {0b00100, 0b00100, 0b01000, 0b00000, 0b00000, 0b00000, 0b00000}},
        {'(', {0b00010, 0b00100, 0b01000, 0b01000, 0b01000, 0b00100, 0b00010}},
        {')', {0b01000, 0b00100, 0b00010, 0b00010, 0b00010, 0b00100, 0b01000}},
        {'[', {0b01110, 0b01000, 0b01000, 0b01000, 0b01000, 0b01000, 0b01110}},
        {']', {0b01110, 0b00010, 0b00010, 0b00010, 0b00010, 0b00010, 0b01110}},
        {'%', {0b11001, 0b11001, 0b00010, 0b00100, 0b01000, 0b10011, 0b10011}},
        {'+', {0b00000, 0b00100, 0b00100, 0b11111, 0b00100, 0b00100, 0b00000}},
        {'>', {0b10000, 0b01000, 0b00100, 0b00010, 0b00100, 0b01000, 0b10000}},
        {'<', {0b00010, 0b00100, 0b01000, 0b10000, 0b01000, 0b00100, 0b00010}},
        {'=', {0b00000, 0b00000, 0b11111, 0b00000, 0b11111, 0b00000, 0b00000}},
        {'_', {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111}},
    };

    const Glyph& FindGlyph(char letter) {
        letter = (char)std::toupper((unsigned char)letter);
        for (const Glyph& glyph : FONT) {
            if (glyph.letter == letter) return glyph;
        }
        for (const Glyph& glyph : FONT) {
            if (glyph.letter == '?') return glyph;   // a letter the font does not have is shown as '?'
        }
        return FONT[0];
    }

    void SetColor(SDL_Renderer* renderer, SDL_Color color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }

    void FillRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
        SetColor(renderer, color);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer, &rect);
    }

    void DrawGlyphs(SDL_Renderer* renderer, const std::string& text, int x, int y, int scale, SDL_Color color) {
        SetColor(renderer, color);
        for (char letter : text) {
            const Glyph& glyph = FindGlyph(letter);
            for (int row = 0; row < GLYPH_H; row++) {
                int col = 0;
                while (col < GLYPH_W) {   // join squares that touch in one row into a single rectangle
                    if (!(glyph.rows[row] & (1 << (GLYPH_W - 1 - col)))) { col++; continue; }
                    int start = col;
                    while (col < GLYPH_W && (glyph.rows[row] & (1 << (GLYPH_W - 1 - col)))) col++;
                    SDL_Rect rect = {x + start * scale, y + row * scale, (col - start) * scale, scale};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
            x += ADVANCE * scale;
        }
    }
}

namespace UI {

int TextWidth(const std::string& text, int scale) {
    if (text.empty()) return 0;
    return ((int)text.size() * ADVANCE - 1) * scale;   // minus the gap after the last letter
}

int TextHeight(int scale) {
    return GLYPH_H * scale;
}

void DrawText(SDL_Renderer* renderer, const std::string& text, int x, int y, int scale, SDL_Color color, bool shadow) {
    if (shadow) DrawGlyphs(renderer, text, x + scale, y + scale, scale, SDL_Color{90, 90, 110, 255});
    DrawGlyphs(renderer, text, x, y, scale, color);
}

void DrawTextCentered(SDL_Renderer* renderer, const std::string& text, int centerX, int y, int scale, SDL_Color color, bool shadow) {
    DrawText(renderer, text, centerX - TextWidth(text, scale) / 2, y, scale, color, shadow);
}

std::vector<std::string> WrapText(const std::string& text, int maxWidth, int scale) {
    std::vector<std::string> lines;
    std::string line;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find(' ', pos);
        if (end == std::string::npos) end = text.size();
        std::string word = text.substr(pos, end - pos);
        std::string tryLine = line.empty() ? word : line + " " + word;
        if (!line.empty() && TextWidth(tryLine, scale) > maxWidth) {
            lines.push_back(line);
            line = word;
        } else {
            line = tryLine;
        }
        pos = end + 1;
    }
    if (!line.empty()) lines.push_back(line);
    return lines;
}

int CountLetters(const std::vector<std::string>& lines) {
    int total = 0;
    for (const std::string& line : lines) total += (int)line.size();
    return total;
}

void DrawLinesCentered(SDL_Renderer* renderer, const std::vector<std::string>& lines, int centerX, int y, int scale, SDL_Color color, int charLimit) {
    int left = charLimit;
    for (const std::string& line : lines) {
        if (charLimit >= 0 && left <= 0) break;
        std::string shown = (charLimit >= 0 && left < (int)line.size()) ? line.substr(0, left) : line;
        // Position by the full line, so the letters stay where they are while the line is being typed.
        DrawText(renderer, shown, centerX - TextWidth(line, scale) / 2, y, scale, color);
        left -= (int)line.size();
        y += TextHeight(scale) + 2 * scale;
    }
}

void DrawHealthLabel(SDL_Renderer* renderer, const std::string& name, int health, int maxHealth, int centerX, int bottomY, bool alive) {
    const int barW = 120;
    const int barH = 12;
    const int nameScale = 2;
    int barX = centerX - barW / 2;
    int barY = bottomY - barH;
    int nameY = barY - 3 - TextHeight(nameScale);

    DrawTextCentered(renderer, name, centerX, nameY, nameScale, alive ? DARK : GRAY);

    if (health < 0) health = 0;
    if (health > maxHealth) health = maxHealth;
    float fraction = maxHealth > 0 ? (float)health / (float)maxHealth : 0.0f;

    FillRect(renderer, barX - 2, barY - 2, barW + 4, barH + 4, DARK);       // border
    FillRect(renderer, barX, barY, barW, barH, SDL_Color{70, 70, 80, 255}); // empty part
    SDL_Color fill = fraction > 0.5f ? SDL_Color{80, 200, 90, 255}
                   : fraction > 0.25f ? SDL_Color{240, 190, 50, 255}
                                      : SDL_Color{220, 60, 60, 255};
    FillRect(renderer, barX, barY, (int)(barW * fraction), barH, fill);

    std::string numbers = alive ? std::to_string(health) + "/" + std::to_string(maxHealth) : "DEFEATED";
    DrawTextCentered(renderer, numbers, centerX, barY + (barH - TextHeight(1)) / 2, 1, WHITE, true);
}

void DrawPromptBar(SDL_Renderer* renderer, const std::string& heading, const std::string& controls) {
    int w = 0, h = 0;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    const int barH = 52;
    FillRect(renderer, 0, h - barH, w, barH, SDL_Color{20, 20, 40, 255});
    FillRect(renderer, 0, h - barH, w, 3, GOLD);
    DrawTextCentered(renderer, heading, w / 2, h - barH + 9, 2, WHITE);
    DrawTextCentered(renderer, controls, w / 2, h - barH + 30, 2, GOLD);
}

void DrawOverlay(SDL_Renderer* renderer, const std::string& title, const std::string& line1, const std::string& line2) {
    int w = 0, h = 0;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    FillRect(renderer, 0, 0, w, h, SDL_Color{10, 10, 30, 210});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    DrawTextCentered(renderer, title, w / 2, h / 2 - 80, 10, GOLD, true);
    DrawTextCentered(renderer, line1, w / 2, h / 2 + 20, 3, WHITE);
    DrawTextCentered(renderer, line2, w / 2, h / 2 + 70, 2, GRAY);
}

int TextureHeight(SDL_Texture* texture) {
    int w = 0, h = 0;
    if (texture != nullptr) SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    return h;
}

void DrawSprite(SDL_Renderer* renderer, SDL_Texture* texture, float centerX, float centerY, float scale) {
    if (texture == nullptr) return;
    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);   // ask the picture how big it is
    SDL_Rect dst;
    dst.w = (int)(w * scale);
    dst.h = (int)(h * scale);
    dst.x = (int)(centerX - dst.w / 2);   // the position is the middle of the picture, so move left by half its width...
    dst.y = (int)(centerY - dst.h / 2);   // ...and up by half its height
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

}
