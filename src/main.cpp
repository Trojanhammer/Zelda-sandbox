#include "Player.h"
#include "graphics/graphics.h"
#include "ui/ui.h"

#include <SDL.h>
#include <memory>
#include <cmath>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Chemistry sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          Gfx::WINDOW_W, Gfx::WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    auto MainPlayer = std::make_unique<Player>(Weapon("Master Sword",0,0));
    MainPlayer -> weapon.posX = (MainPlayer -> posX) + 50;
    MainPlayer -> weapon.posY = (MainPlayer -> posY) + 50;
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
            
            // TODO (you): the other keys (throw, recall) go here
        }

        // TODO (you): the time and the physics: work out how much time has passed and move your objects.
        SDL_SetRenderDrawColor(renderer, 245, 245, 240, 255);
        SDL_RenderClear(renderer);

        // Placeholder, so that you can see that the drawing works. Delete it and draw your own objects with the same calls.
        Gfx::DrawGround(renderer, 480);
        Gfx::DrawHand(renderer, MainPlayer -> posX, MainPlayer -> posY);
        Gfx::DrawSword(renderer, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, std::atan2(MainPlayer -> weapon.vY, MainPlayer -> weapon.vX));
        // pointing up and to the right
        Gfx::DrawMarker(renderer, 300, 350, SDL_Color{200, 50, 50, 255});
        UI::DrawTextCentered(renderer, "RECALL", Gfx::WINDOW_W / 2, 10, 2, UI::DARK);
        UI::DrawText(renderer, "ESC: QUIT", 20, 500, 2, UI::DARK);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
