#pragma once
#include <SDL.h>
#include <string>
#include <vector>

// Everything that puts text and bars on the screen.
//
// SDL2 can draw pictures but not text, and SDL_ttf (the usual helper) needs a font file and an extra install.
// So the letters here are drawn by the program itself, out of small filled squares: every letter is a 5 x 7 grid of
// squares, and `scale` says how many pixels one square is. scale 2 = each letter is 10 x 14 pixels.
// Capital letters only (lowercase is shown as capitals). If you ever want a nicer font, only ui.cpp has to change.
namespace UI {
    const SDL_Color WHITE = {255, 255, 255, 255};
    const SDL_Color DARK = {30, 30, 60, 255};
    const SDL_Color GRAY = {130, 130, 140, 255};
    const SDL_Color GOLD = {255, 205, 70, 255};
    const SDL_Color RED = {200, 50, 50, 255};

    // x = left edge. A letter is 5 squares wide plus 1 square of gap, so 3 letters at scale 2 = 3 * 6 * 2 - 2 = 34 pixels.
    int TextWidth(const std::string& text, int scale);
    int TextHeight(int scale);   // 7 squares

    // x, y = top-left corner of the first letter. shadow = a gray copy one square down and right (easier to read).
    void DrawText(SDL_Renderer* renderer, const std::string& text, int x, int y, int scale, SDL_Color color, bool shadow = false);
    void DrawTextCentered(SDL_Renderer* renderer, const std::string& text, int centerX, int y, int scale, SDL_Color color, bool shadow = false);

    // Cuts a sentence into lines that are at most maxWidth pixels wide (never cuts inside a word).
    std::vector<std::string> WrapText(const std::string& text, int maxWidth, int scale);
    // Draws lines one under the other, centered on centerX. Only the first `charLimit` letters (over all lines) are
    // drawn, which is how the typing effect works: raise charLimit a little every frame. -1 = draw everything.
    void DrawLinesCentered(SDL_Renderer* renderer, const std::vector<std::string>& lines, int centerX, int y, int scale, SDL_Color color, int charLimit = -1);
    int CountLetters(const std::vector<std::string>& lines);

    // Name above a health bar with "80/80" inside it. centerX = middle of the label, bottomY = where the bar ends.
    // Clamps health to 0..maxHealth, so a hit that goes below zero still shows 0/80. alive = false shows DEFEATED.
    void DrawHealthLabel(SDL_Renderer* renderer, const std::string& name, int health, int maxHealth, int centerX, int bottomY, bool alive);

    // A dark strip across the bottom of the window with two lines of text (a heading and the controls).
    void DrawPromptBar(SDL_Renderer* renderer, const std::string& heading, const std::string& controls);

    // Dims the whole window and puts a big title in the middle (game over / you win).
    void DrawOverlay(SDL_Renderer* renderer, const std::string& title, const std::string& line1, const std::string& line2);

    int TextureHeight(SDL_Texture* texture);

    // Draws a picture with its middle at (centerX, centerY), `scale` times its own size. A missing picture is skipped, no crash.
    // (Same as the DrawSprite that used to live in main.cpp.)
    void DrawSprite(SDL_Renderer* renderer, SDL_Texture* texture, float centerX, float centerY, float scale);
}
