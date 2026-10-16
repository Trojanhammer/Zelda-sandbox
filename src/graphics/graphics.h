#pragma once
#include <SDL.h>
#include <vector>

// Everything that only draws. None of it moves or decides anything: you give it a position and it paints.
// Coordinates are pixels in the window (960 x 540): x grows to the right, y grows DOWNWARDS, (0,0) is the top-left corner.
namespace Gfx {
    const int WINDOW_W = 960;
    const int WINDOW_H = 540;
    const int WORLD_W = 6000;   // how wide the world is (the window shows 960 of it; see camera.h)

    // The ground: a brown block from groundY down to the bottom of the window, along the whole world (WORLD_W px, plus a little
    // outside both ends so the window never shows an edge of it).
    void DrawGround(SDL_Renderer* renderer, float groundY);

    // Marks on the ground so that you can see the camera move and how far something flew: a small tick every 100 px, a longer one
    // with the number (the world x) every 500 px, and a tall post at both ends of the world. Draw it right after DrawGround.
    void DrawWorldMarks(SDL_Renderer* renderer, float groundY);

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
    // flipHorizontal = mirror the picture first, and then turn it (so a picture that points left now points right at angle 0).
    void DrawSpriteRotated(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float angleRadians, float scale = 1.0f, bool flipHorizontal = false);

    // Link with moving legs. walking = false draws him standing (the picture as it is); walking = true swings his two legs
    // while the body bobs. The swing follows how far he has walked (x), so his feet do not slide on the ground:
    // one full step (both legs) per 80 px. flipHorizontal = face left (the legs then swing as if walking forward to the left).
    // The numbers inside are measured on assets/link.png (55 x 112): legs start at row 73, the two legs are cut apart at column 28.
    void DrawWalker(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, bool walking, bool flipHorizontal = false, float scale = 1.0f);

    // Loads a picture like LoadTexture, but every visible pixel gets one colour (the see-through parts stay see-through): a flat
    // silhouette of the picture. DrawRecallEffect uses it for the glow. Returns nullptr, and prints why, if the file is missing.
    SDL_Texture* LoadSilhouette(SDL_Renderer* renderer, const char* path, SDL_Color color);

    // The look of a recall, a simple version of the one in the game: a flickering yellow rim around the sword, a see-through
    // copy of it where it will end up, and the line of the path it goes back along. `path` = the recorded dots from where the
    // throw began up to where the sword is now, in that order. `silhouette` comes from LoadSilhouette. Call it BEFORE drawing
    // the sword itself, so the rim sits behind it. Does nothing if silhouette is nullptr.
    void DrawRecallEffect(SDL_Renderer* renderer, SDL_Texture* silhouette, float swordX, float swordY, float swordAngle, const std::vector<SDL_FPoint>& path);

    // The same look of a recall for the metal box (it has no picture to turn into a silhouette, so it is drawn from rectangles): a
    // flickering golden frame around the box, a see-through golden box where it will end up (the first dot of the path), and the
    // golden line of the path it goes back along. `path` = the recorded dots from where it was let go up to where it is now, in that
    // order, `size` is the box's size in px (the same as DrawBox). Call it BEFORE DrawBox, so the frame sits behind the box.
    void DrawBoxRecallEffect(SDL_Renderer* renderer, float x, float y, float size, const std::vector<SDL_FPoint>& path);

    // The look of a box that is frozen in time (stasis), drawn ON TOP of DrawBox: a yellow tint and frame, an arrow that shows where
    // it will fly when time starts again (it points the way of the speed the hits stored, storedVX and storedVY in px/s, and the
    // harder it was hit the longer it is), a bar above it with the time left, and a bright flash on a hit.
    // Before the first hit there is no speed yet, so a short faint arrow shows the aim (aimDegrees, 0 = right, 90 = up) instead.
    // timeLeft goes from 1 (just frozen) to 0 (time is up) and flash from 1 (a hit just happened) to 0. Nothing moves in it: frozen.
    void DrawBoxStasisEffect(SDL_Renderer* renderer, float x, float y, float size, float storedVX, float storedVY,
                             float aimDegrees, float timeLeft, float flash);

    // The three pictures of the monster (all of them face LEFT, they are mirrored when it looks right). Any of them can be nullptr:
    // without `body` the monster is drawn from rectangles, without `club` it has no club, without `bodyRed` it does not turn red.
    struct MonsterSprites {
        SDL_Texture* body = nullptr;      // assets/bokoblin.png, LoadTexture
        SDL_Texture* club = nullptr;      // assets/club.png, LoadTexture (the heavy end on the left, the handle on the right)
        SDL_Texture* bodyRed = nullptr;   // the body in flat red: LoadSilhouette("assets/bokoblin.png", red). Laid over the body while it winds up
    };

    // A monster (a Bokoblin): a red body with horns, a club in its fist, and a health bar above it. Its middle is (x, y), the same
    // 72 px tall box as before, so with its middle at y = 426 its feet are on the ground at 462 (the picture is taller than that,
    // it stands on the ground and reaches higher). facingLeft = looking to the left. health goes from 1 (full) to 0 (the bar is empty).
    // windup goes from 0 to 1 while it gets ready to hit: the club goes up and back over its head and the body turns redder, so the
    // player can see the hit coming (use the attack timer of the monster, 0 while it is not attacking).
    void DrawMonster(SDL_Renderer* renderer, const MonsterSprites& sprites, float x, float y, bool facingLeft, float health, float windup);

    // Link's hearts for the screen (draw them after camera.End): one heart for every 20 health, so a health of 100 is five full hearts,
    // 70 is three and a half, and what is lost is shown as grey hearts. (x, y) is the top-left corner of the first heart.
    void DrawHearts(SDL_Renderer* renderer, int x, int y, float health, float maxHealth);

    // The fire on the ground for the updraft. Its base sits at (x, groundY); `radius` is half of its width on the grass (50 for a
    // circle of 100 px). `intensity` runs from 0 to 1 (how strong the updraft is right now, force / max force): a weak fire is
    // small and a strong one fills the circle. `flame` is the flame picture (its bottom edge is put on the ground), or nullptr
    // to draw a flame from triangles. Also draws a flat ring on the grass that shows how far the updraft reaches.
    void DrawFire(SDL_Renderer* renderer, SDL_Texture* flame, float x, float groundY, float radius, float intensity);

    // The rising air above the fire: a faint column and streaks that move up, faster and brighter when the updraft is
    // stronger. columnHeight is how far up it reaches (px). Draw it BEFORE the things that are lifted.
    void DrawUpdraftColumn(SDL_Renderer* renderer, float x, float groundY, float radius, float columnHeight, float intensity);

    // What is thrown: the fireball picture turned along its flight (the picture points RIGHT with its tail on the left, like the
    // sword: angle = atan2(speedY, speedX)), or nullptr to draw a ball of fire with a tail.
    void DrawFireball(SDL_Renderer* renderer, SDL_Texture* fireball, float x, float y, float angle);

    // The big metal box for magnesis, a square of `size` px with its MIDDLE at (x, y) (use the box's posX, posY; size 104 puts its
    // bottom on the ground when groundY is 410). Drawn from rectangles: steel with a lit top-left edge, a crossed panel and four rivets.
    // held = true while magnesis has it: a red glow that pulses around it, with a thin blue rim (the two ends of a magnet).
    void DrawBox(SDL_Renderer* renderer, float x, float y, float size, bool held);

    // A small cross with its middle at (x, y). Handy to look at the recorded dots of a path.
    void DrawMarker(SDL_Renderer* renderer, float x, float y, SDL_Color color);
}
