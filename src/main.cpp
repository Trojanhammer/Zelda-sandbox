#include "Player.h"
#include "graphics/graphics.h"
#include "ui/ui.h"
#include "input/gamepad.h"

#include <SDL.h>
#include <memory>
#include <cmath>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    Gamepad::Init();
    SDL_Window* window = SDL_CreateWindow("Zelda sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          Gfx::WINDOW_W, Gfx::WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    auto MainPlayer = std::make_unique<Player>(Weapon("Master Sword"));
    SDL_Texture* linkTexture = Gfx::LoadTexture(renderer, "assets/link.png");           // loaded once; nullptr (nothing drawn) if the file is missing
    SDL_Texture* swordTexture = Gfx::LoadTexture(renderer, "assets/master-sword.png");
    bool running = true;
    SDL_Event event;
    float angleDegrees=0.0f;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (running) {
        float deltaTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastTime).count();
        lastTime = std::chrono::high_resolution_clock::now();
        while (SDL_PollEvent(&event)) {
            Gamepad::HandleEvent(event);
            Gamepad::Action action = Gamepad::ActionFromEvent(event);
            if(action == Gamepad::ThrowAction && MainPlayer -> weapon.state == Weapon::Held){
                MainPlayer -> weapon.state = Weapon::Start_Throw;
            }
            else if(action == Gamepad::RecallAction && (MainPlayer -> weapon.state == Weapon::Thrown || MainPlayer -> weapon.state == Weapon::Idle)){
                MainPlayer -> weapon.state = Weapon::Recalling;
            }

            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
            
            // TODO (you): the other keys (throw, recall) g  o here
            // make state to recall once a button is clicked either when idle or otw during threw .
            // make state to start throw when once a button is clicked
        }
        float stickAngle;
        if(Gamepad::AimAngle(stickAngle)) angleDegrees = stickAngle;

        // TODO (you): the time and the physics: work out how much time has passed and move your objects.

        if(MainPlayer -> weapon.state == Weapon::Start_Throw){
            MainPlayer -> Throw(angleDegrees);
        }
        else if(MainPlayer -> weapon.state == Weapon::Thrown){
            MainPlayer -> weapon.Update(deltaTime);
        }
        else if(MainPlayer -> weapon.state == Weapon::Recalling){
            MainPlayer -> weapon.Recall(deltaTime);
        }
        else if(MainPlayer -> weapon.state == Weapon::Held || MainPlayer -> weapon.state == Weapon::Idle){
            // do nothing
        }

        SDL_SetRenderDrawColor(renderer, 245, 245, 240, 255);
        SDL_RenderClear(renderer);

        Gfx::DrawGround(renderer, 462);   // the sword lands with its middle at y = 450 (Weapon.cpp) and is 25 px tall, so the surface is at 462
        Gfx::DrawSprite(renderer, linkTexture, MainPlayer -> posX, MainPlayer -> posY);
        // The sword points along the aim while it is held, and the way it moves while it flies (the picture points right at angle 0).
        // Screen y points down, so an aim of 30 degrees upward is a rotation of -30.
        float swordAngle = (MainPlayer -> weapon.state == Weapon::Held) ? -angleDegrees * 3.14159265f / 180.0f
                                                                        : std::atan2(MainPlayer -> weapon.vY, MainPlayer -> weapon.vX);
        if(MainPlayer -> weapon.state == Weapon::Held){
            Gfx::DrawAimLine(renderer, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, angleDegrees);
        }
        Gfx::DrawSpriteRotated(renderer, swordTexture, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, swordAngle);
        UI::DrawText(renderer, Gamepad::Connected() ? "CONTROLLER OK" : "NO CONTROLLER", 20, 20, 2, UI::DARK);
        UI::DrawTextCentered(renderer, "RECALL", Gfx::WINDOW_W / 2, 10, 2, UI::DARK);
        UI::DrawText(renderer, "ESC: QUIT", 20, 500, 2, UI::DARK);

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(linkTexture);     // before the renderer goes away
    SDL_DestroyTexture(swordTexture);
    Gamepad::Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
