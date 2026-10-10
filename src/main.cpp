#include "Player.h"
#include "graphics/graphics.h"
#include "ui/ui.h"
#include "input/gamepad.h"
#include "Fire.h"

#include <SDL.h>
#include <memory>
#include <cmath>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    Gamepad::Init();
    SDL_Window* window = SDL_CreateWindow("Zelda sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          Gfx::WINDOW_W, Gfx::WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    auto MainPlayer = std::make_unique<Player>(Weapon());
    SDL_Texture* linkTexture = Gfx::LoadTexture(renderer, "assets/link.png");           // loaded once; nullptr (nothing drawn) if the file is missing
    SDL_Texture* swordTexture = Gfx::LoadTexture(renderer, "assets/master-sword.png");
    SDL_Texture* swordGlow = Gfx::LoadSilhouette(renderer, "assets/master-sword.png", SDL_Color{255, 205, 40, 255});   // the same shape in flat yellow, for the recall glow
    SDL_Texture* flameTexture = Gfx::LoadTexture(renderer, "assets/flame.png");         // the fire on the grass
    SDL_Texture* fireballTexture = Gfx::LoadTexture(renderer, "assets/fireball.png");   // what is thrown
    bool running = true;
    SDL_Event event;
    float angleDegrees=0.0f;
    bool PathChoice = true; // true to the right and vice versa
    std::unique_ptr<Fire>fire = nullptr;
    std::unique_ptr<Updraft>updraft = nullptr;
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (running) {
//---------------------------Input Check --------------------------------------------------------------
        float deltaTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastTime).count();
        lastTime = std::chrono::high_resolution_clock::now();
        
        // "while" here is to capture event that changes.example clicking "square" for one second just register one time ,not 60 times if 60 fps.
        while (SDL_PollEvent(&event)) {
            Gamepad::HandleEvent(event);
            Gamepad::Action action = Gamepad::ActionFromEvent(event);
            if(action == Gamepad::ThrowAction && MainPlayer -> weapon.state == Weapon::Held){
                MainPlayer -> weapon.state = Weapon::Start_Throw;
            }
            else if(action == Gamepad::RecallAction && (MainPlayer -> weapon.state == Weapon::Thrown || MainPlayer -> weapon.state == Weapon::Idle) && (!MainPlayer -> weapon.RecallCoord.empty())){
                MainPlayer -> weapon.state = Weapon::Recalling;
            }
            else if(action == Gamepad::FireAction){
                fire = std::make_unique<Fire>();
            }
            // set fire(initialize the obj)
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }
        float stickAngle;
        if(Gamepad::AimAngle(stickAngle)) angleDegrees = stickAngle;
        
        const Uint8* keys = SDL_GetKeyboardState(nullptr); // read which key are hold down right now, read every frame
        bool leftHeld = keys[SDL_SCANCODE_LEFT];
        bool rightHeld = keys[SDL_SCANCODE_RIGHT];

        if(rightHeld){
            PathChoice = true;
            MainPlayer -> state = Player::Walk;
        }
        else if (leftHeld){
            PathChoice = false;
            MainPlayer -> state = Player::Walk;
        }
        else{
            MainPlayer -> state = Player::Idle;
        }
        
        // TODO (you): the time and the physics: work out how much time has passed and move your objects.
        if(MainPlayer -> state == Player::Walk){
            MainPlayer -> RightWalk = PathChoice;
            MainPlayer -> Walking(deltaTime);
        }
        if(fire!=nullptr){
            if(fire -> state == Fire::Start_Throw){
                fire -> posX = MainPlayer -> posX + MainPlayer -> Player::HandX;
                fire -> posY = MainPlayer -> posY + MainPlayer -> Player::HandY;
                MainPlayer -> Throw(angleDegrees,*fire);
                fire -> state = Fire::Thrown; 
            }
            else if(fire -> state == Fire::Thrown){
                fire -> Update(deltaTime);
                if(fire -> vX ==0 && fire -> vY ==0){
                    fire -> state = Fire::Firing;
                if(!fire -> isUpDraft){
                    updraft = std::make_unique<Updraft>();             
                }
            }
            }
            else if(fire -> state == Fire::Firing){
                updraft -> Update(deltaTime);
                if(std::abs(fire -> posX - MainPlayer -> posX) <= 10){
                    updraft -> Push(deltaTime,*MainPlayer);
                    if(MainPlayer -> weapon.state == Weapon::Held){
                        MainPlayer -> weapon.posY = MainPlayer -> posY + MainPlayer -> HandY;
                    }
                }
                if(updraft -> isFinished){
                    fire.reset();
                    updraft.reset();
                    // delete both fire object and pointer here/
            }
            }
        }
        // gravity pull every frame
        MainPlayer -> OpposeGravity(deltaTime, MainPlayer -> groundY); // 
        
        if(MainPlayer -> weapon.state == Weapon::Start_Throw){
            MainPlayer -> Throw(angleDegrees,MainPlayer -> weapon);
            MainPlayer -> weapon.state = Weapon::Thrown;
        }
        else if(MainPlayer -> weapon.state == Weapon::Thrown){
            MainPlayer -> weapon.RecallCoord.push_back({MainPlayer -> weapon.posX,MainPlayer -> weapon.posY, MainPlayer -> weapon.RecallClock});
            MainPlayer -> weapon.Update(deltaTime);
            if((MainPlayer -> weapon.vX ==0) && (MainPlayer -> weapon.vY ==0)){
                MainPlayer -> weapon.state = Weapon::Idle;
            }
            MainPlayer -> weapon.RecallClock +=deltaTime;
        }
        else if(MainPlayer -> weapon.state == Weapon::Recalling){
            MainPlayer -> weapon.Recall(deltaTime,MainPlayer -> posX);
        }
        else if(MainPlayer -> weapon.state == Weapon::Fall){
                MainPlayer -> weapon.Fallen(deltaTime);
        }
        else if(MainPlayer -> weapon.state == Weapon::Held){
            // do nothing
        }
        else if (MainPlayer -> weapon.state == Weapon::Idle){
            if((MainPlayer -> weapon.RecallClock < MainPlayer -> weapon.RecallLimit) &&  MainPlayer -> weapon.RecallClock > 0.0f){
                MainPlayer -> weapon.RecallClock += deltaTime;
                MainPlayer -> weapon.RecallCoord.push_back({MainPlayer -> weapon.posX,MainPlayer -> weapon.posY,MainPlayer -> weapon.RecallClock});
            }
            else if(MainPlayer -> weapon.RecallClock >= MainPlayer -> weapon.RecallLimit){
                MainPlayer -> weapon.RecallCoord.clear();
                MainPlayer -> weapon.RecallClock = 0.0f;
            }
        }

        SDL_SetRenderDrawColor(renderer, 245, 245, 240, 255);
        SDL_RenderClear(renderer);

        Gfx::DrawGround(renderer, 462);   // the sword lands with its middle at y = 450 (Weapon.cpp) and is 25 px tall, so the surface is at 462
        if(fire != nullptr && fire -> state == Fire::Firing && updraft != nullptr){   // the fire on the grass and its rising air, behind Link
            // How strong it looks, 0 to 1. While it fades the force starts at his weight (686), so measure it against that and not
            // against the maximum, otherwise the flame would jump from full to two thirds at 10 s.
            float intensity = updraft -> decayStarted ? updraft -> updraft_force / Updraft::gravity_force
                                                      : updraft -> updraft_force / Updraft::maxUpdraftForce;
            Gfx::DrawUpdraftColumn(renderer, fire -> posX, 462, 50, 250, intensity);
            Gfx::DrawFire(renderer, flameTexture, fire -> posX, 462, 50, intensity);
        }
        Gfx::DrawWalker(renderer, linkTexture, MainPlayer -> posX, MainPlayer -> posY, MainPlayer -> state == Player::Walk);   // legs swing while he walks
        if(fire != nullptr && fire -> state == Fire::Thrown){   // the fireball while it flies, turned the way it moves
            Gfx::DrawFireball(renderer, fireballTexture, fire -> posX, fire -> posY, std::atan2(fire -> vY, fire -> vX));
        }
        // The sword points along the aim while it is held, and the way it moves while it flies (the picture points right at angle 0).
        // Screen y points down, so an aim of 30 degrees upward is a rotation of -30.
        float swordAngle = (MainPlayer -> weapon.state == Weapon::Held) ? -angleDegrees * 3.14159265f / 180.0f
                                                                        : std::atan2(MainPlayer -> weapon.vY, MainPlayer -> weapon.vX);
        if(MainPlayer -> weapon.state == Weapon::Held){
            Gfx::DrawAimLine(renderer, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, angleDegrees);
        }
        if(MainPlayer -> weapon.state == Weapon::Recalling){   // glow + the path back, behind the sword
            std::vector<SDL_FPoint> pathBack;                  // the recorded dots from the start of the throw up to where the sword is now
            for(const auto& dot : MainPlayer -> weapon.RecallCoord){
                if(dot.time <= MainPlayer -> weapon.RecallClock) pathBack.push_back({dot.posX, dot.posY});
            }
            Gfx::DrawRecallEffect(renderer, swordGlow, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, swordAngle, pathBack);
        }
        Gfx::DrawSpriteRotated(renderer, swordTexture, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, swordAngle);
        UI::DrawText(renderer, Gamepad::Connected() ? "CONTROLLER OK" : "NO CONTROLLER", 20, 20, 2, UI::DARK);
        UI::DrawTextCentered(renderer, "RECALL", Gfx::WINDOW_W / 2, 10, 2, UI::DARK);
        UI::DrawText(renderer, "ESC: QUIT", 20, 500, 2, UI::DARK);

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(linkTexture);     // before the renderer goes away
    SDL_DestroyTexture(swordTexture);
    SDL_DestroyTexture(swordGlow);
    SDL_DestroyTexture(flameTexture);
    SDL_DestroyTexture(fireballTexture);
    Gamepad::Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
