#include "Player.h"
#include "graphics/graphics.h"
#include "graphics/camera.h"
#include "ui/ui.h"
#include "input/gamepad.h"
#include "Fire.h"
#include "Magnesis.h"
#include "Stasis.h"

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
    float aimAngle=0.0f;   // the direction the left stick points (0 to 180 degrees): used for throwing, aiming and hitting
    bool PathChoice = true; // true to the right and vice versa

    std::unique_ptr<Fire> fire = nullptr;
    std::unique_ptr<Magnesis> magnesis = nullptr;
    std::unique_ptr<Stasis> stasis = nullptr;

    auto metal_box = std::make_unique<Box>();
    Camera camera;   // see graphics/camera.h
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
            else if(action == Gamepad::RecallAction && (MainPlayer -> weapon.state == Weapon::Thrown || MainPlayer -> weapon.state == Weapon::Idle) && (!MainPlayer -> weapon.recall.RecallCoord.empty())){
                MainPlayer -> weapon.state = Weapon::Recalling;
            }
            else if(action == Gamepad::BoxRecallAction && (metal_box -> state == Box::Idle || metal_box -> state == Box::Fly) && !metal_box -> recall.RecallCoord.empty()){
                metal_box -> state = Box::Recalling;
            }
            else if(action == Gamepad::StasisAction && stasis == nullptr && (metal_box -> state == Box::Idle || metal_box -> state == Box::Fly)){
                stasis = std::make_unique<Stasis>();
                metal_box -> state = Box::Stasis;     // beku bermula di sini, bukan di hujung
                metal_box -> vX = 0;
                metal_box -> vY = 0;
            }
            else if(action == Gamepad::FireAction){
                fire = std::make_unique<Fire>();
            }
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr); // read which key are hold down right now, read every frame
        bool magnesisHeld = Gamepad::MagnesisHeld() || keys[SDL_SCANCODE_R]; // runs on every frame to know false/true.If dont click recall,it will be false
        float sx =0, sy=0; // speed x & y of analog
        Gamepad::RightStick(sx,sy);
        sy = -sy; // right stick will give x and y value based on normal ones(atas (+))
        if(keys[SDL_SCANCODE_UP]) sy = -1;
        if(keys[SDL_SCANCODE_DOWN]) sy = 1;
        if(magnesisHeld){
            if(magnesis == nullptr){ // initialized once
                magnesis = std::make_unique<Magnesis>();
                metal_box -> state = Box::Magnesis;
            }
            if(sx !=0 || sy !=0){
                float magnesisAngle = std::atan2(sy,sx) * 180.0f / 3.14159265f; // calculate angle of target pos and before.
                magnesis -> Update(*metal_box, deltaTime, magnesisAngle); // determine vX & vY
            }
            else{
                metal_box -> vX = 0;
                metal_box -> vY = 0;
                metal_box -> lift = PhysicsObject::gravity;
            }
        }
        else if(magnesis !=nullptr){ // only delete & freed & change state to Fly when magnesisHeld is false and magnesis is exist (when magnesis button stop)
            magnesis.reset(); // delete ptr and freed the object
            metal_box -> state = Box::Fly;
        }
        float stickAngle;
        if(Gamepad::AimAngle(stickAngle)) aimAngle = stickAngle;
        bool hit = Gamepad::HitPressed();   // asked on every frame (it remembers the trigger from the frame before), then only counted when he is close
        if(std::abs(MainPlayer -> posX - metal_box -> posX) >= 20){
            hit = false;   // too far from the box: not a hit
        }
        //sini ada button untuk tekan stasis and tekan hit sekali
        
        if(stasis != nullptr){
            if(stasis -> UpdateStasis(hit,deltaTime,aimAngle, *metal_box)){
                metal_box -> state = Box::Fly;
                stasis.reset();
            }
        }

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
        
// ------------ PHYSICS -----------------------------------------------------------
        if(MainPlayer -> state == Player::Walk){
            MainPlayer -> RightWalk = PathChoice;
            MainPlayer -> Walking(deltaTime);
        }

        if(fire!=nullptr){ // mark for deletion inside , but freed from outside
            fire -> UpdateFire(deltaTime,aimAngle,*MainPlayer);
            if(!(fire -> isAlive))
            {
                fire.reset();
            }
        }

        MainPlayer -> UpdatePosition(deltaTime);

        metal_box -> UpdateBox(deltaTime);
        
        MainPlayer -> UpdateWeapon(deltaTime,aimAngle);


//---------------- RENDER -------------------------------------------------------------------------------
        SDL_SetRenderDrawColor(renderer, 245, 245, 240, 255);
        SDL_RenderClear(renderer);

        // The camera: the window follows Link, and follows the box while it flies (moving sideways), and keeps looking at it for
        // 1.5 s after it stops, so you can see where it landed before the window goes back to Link.
        static float cameraLinger = 0.0f;
        bool boxFlying = (metal_box -> state == Box::Fly && std::abs(metal_box -> vX) > 20.0f);
        cameraLinger = boxFlying ? 1.5f : (cameraLinger > deltaTime ? cameraLinger - deltaTime : 0.0f);
        float cameraTarget = (cameraLinger > 0.0f) ? metal_box -> posX : MainPlayer -> posX;
        camera.Follow(cameraTarget, deltaTime);
        camera.Begin(renderer);   // from here to camera.End everything is drawn at its world position

        Gfx::DrawGround(renderer, 462);   // the sword lands with its middle at y = 450 (Weapon.cpp) and is 25 px tall, so the surface is at 462
        Gfx::DrawWorldMarks(renderer, 462);   // ticks every 100 px and the distance every 500 px, so you can see the camera move
        if(metal_box -> state == Box::Recalling){   // the golden frame, the path back and the see-through copy, behind the box
            std::vector<SDL_FPoint> boxPathBack;    // the recorded dots from where it was let go up to where it is now
            for(const auto& dot : metal_box -> recall.RecallCoord){
                if(dot.time <= metal_box -> recall.RecallClock) boxPathBack.push_back({dot.posX, dot.posY});
            }
            Gfx::DrawBoxRecallEffect(renderer, metal_box -> posX, metal_box -> posY, 104, boxPathBack);
        }
        Gfx::DrawBox(renderer, metal_box -> posX, metal_box -> posY, 104, magnesis != nullptr);   // the pointer exists only while magnesis holds it
        static float stasisFlash = 0.0f;   // 1 on the frame of a hit, then it fades in about 0.17 s
        stasisFlash = (hit && stasis != nullptr) ? 1.0f : (stasisFlash > deltaTime * 6.0f ? stasisFlash - deltaTime * 6.0f : 0.0f);
        if(stasis != nullptr){   // the yellow tint, the arrow of the speed the hits stored, the time left, on top of the box
            Gfx::DrawBoxStasisEffect(renderer, metal_box -> posX, metal_box -> posY, 104, metal_box -> vX, metal_box -> vY, aimAngle,
                                     1.0f - stasis -> time_record / stasis -> timeLimit, stasisFlash);
        }
        if(fire != nullptr && fire -> state == Fire::Firing && fire -> updraft != nullptr){   // the fire on the grass and its rising air, behind Link
            // How strong it looks, 0 to 1. While it fades the force starts at his weight (686), so measure it against that and not
            // against the maximum, otherwise the flame would jump from full to two thirds at 10 s.
            float intensity = fire -> updraft -> decayStarted ? fire -> updraft -> updraft_force / Updraft::gravity_force
                                                      : fire -> updraft -> updraft_force / Updraft::maxUpdraftForce;
            Gfx::DrawUpdraftColumn(renderer, fire -> posX, 462, 50, 340, intensity);   // 340 px tall: he is held about 319 px up, so his feet (at 462 - 319 = 143) stay inside
            Gfx::DrawFire(renderer, flameTexture, fire -> posX, 462, 50, intensity);
        }
        Gfx::DrawWalker(renderer, linkTexture, MainPlayer -> posX, MainPlayer -> posY, MainPlayer -> state == Player::Walk);   // legs swing while he walks
        if(fire != nullptr && fire -> state == Fire::Thrown){   // the fireball while it flies, turned the way it moves
            Gfx::DrawFireball(renderer, fireballTexture, fire -> posX, fire -> posY, std::atan2(fire -> vY, fire -> vX));
        }
        // The sword points along the aim while it is held, and the way it moves while it flies (the picture points right at angle 0).
        // Screen y points down, so an aim of 30 degrees upward is a rotation of -30.
        float swordAngle = (MainPlayer -> weapon.state == Weapon::Held) ? -aimAngle * 3.14159265f / 180.0f
                                                                        : std::atan2(MainPlayer -> weapon.vY, MainPlayer -> weapon.vX);
        if(MainPlayer -> weapon.state == Weapon::Held){
            Gfx::DrawAimLine(renderer, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, aimAngle);
        }
        if(MainPlayer -> weapon.state == Weapon::Recalling){   // glow + the path back, behind the sword
            std::vector<SDL_FPoint> pathBack;                  // the recorded dots from the start of the throw up to where the sword is now
            for(const auto& dot : MainPlayer -> weapon.recall.RecallCoord){
                if(dot.time <= MainPlayer -> weapon.recall.RecallClock) pathBack.push_back({dot.posX, dot.posY});
            }
            Gfx::DrawRecallEffect(renderer, swordGlow, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, swordAngle, pathBack);
        }
        Gfx::DrawSpriteRotated(renderer, swordTexture, MainPlayer -> weapon.posX, MainPlayer -> weapon.posY, swordAngle);
        camera.End(renderer);   // the text below stays on the screen, it does not move with the world
        UI::DrawText(renderer, Gamepad::Connected() ? "CONTROLLER OK" : "NO CONTROLLER", 20, 20, 2, UI::DARK);
        UI::DrawTextCentered(renderer, "ZELDA SANDBOX", Gfx::WINDOW_W / 2, 10, 2, UI::DARK);
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
