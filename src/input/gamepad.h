#pragma once
#include <SDL.h>

// DualSense (or any gamepad) for the sandbox: the left stick aims, X throws the sword, O recalls it, square throws a fire.
// SDL calls the cross button "A", the circle button "B" and the square button "X" (it names buttons by their place on an Xbox pad).
namespace Gamepad {
    // Call once after SDL_Init(... | SDL_INIT_GAMECONTROLLER): opens the pads that are already plugged in.
    void Init();
    void Quit();

    // Call with every event: opens a pad that is plugged in later and closes one that is unplugged.
    void HandleEvent(const SDL_Event& event);

    bool Connected();   // is any pad open right now?

    // The left stick as a throw angle in degrees, from 0 to 180: 0 = right, 90 = up, 180 = left (the same angles as Throw()).
    // Nothing is thrown into the ground, so a stick pointing below the horizon snaps to a flat throw: down-right gives 0,
    // down-left gives 180, and straight down gives "not aiming".
    // Returns true and fills `degrees` only while the stick is pushed far enough from the middle (the dead zone);
    // otherwise returns false and leaves `degrees` alone, so you can keep the last angle:
    //     float a; if (Gamepad::AimAngle(a)) angleDegrees = a;
    bool AimAngle(float& degrees);

    enum Action { NoAction, ThrowAction, RecallAction, FireAction };
    // What a button press asks for: X (cross) = ThrowAction, O (circle) = RecallAction, square = FireAction,
    // anything else (or not a button press) = NoAction.
    Action ActionFromEvent(const SDL_Event& event);
}
