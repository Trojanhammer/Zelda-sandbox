#include "gamepad.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
    std::vector<SDL_GameController*> pads;   // the pads that are open

    // A stick that rests in the middle still reports small numbers (a worn stick can drift to 10 % or more), and
    // a stick pushed only a little is too uncertain to aim with. Below this much of a full push, count it as "not aiming".
    const float DEAD_ZONE = 0.30f;

    void Open(int deviceIndex) {
        if (!SDL_IsGameController(deviceIndex)) return;
        SDL_GameController* pad = SDL_GameControllerOpen(deviceIndex);   // the same device gives the same pointer back
        if (pad != nullptr && std::find(pads.begin(), pads.end(), pad) == pads.end()) pads.push_back(pad);
    }
}

namespace Gamepad {

void Init() {
    for (int i = 0; i < SDL_NumJoysticks(); i++) Open(i);
}

void Quit() {
    for (SDL_GameController* pad : pads) SDL_GameControllerClose(pad);
    pads.clear();
}

void HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_CONTROLLERDEVICEADDED) {
        Open(event.cdevice.which);                 // for this event `which` is the device index
    } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
        SDL_GameController* gone = SDL_GameControllerFromInstanceID(event.cdevice.which);   // here it is the instance id
        if (gone != nullptr) {
            pads.erase(std::remove(pads.begin(), pads.end(), gone), pads.end());
            SDL_GameControllerClose(gone);
        }
    }
}

bool Connected() {
    return !pads.empty();
}

bool AimAngle(float& degrees) {
    if (pads.empty()) return false;
    // SDL gives -32768 .. 32767. Turn it into -1 .. 1. On the stick, pushing UP gives a NEGATIVE y, so flip it:
    // after the flip, up is positive, which is the direction the angles count in.
    float x = SDL_GameControllerGetAxis(pads[0], SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
    float y = -SDL_GameControllerGetAxis(pads[0], SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
    if (std::hypot(x, y) < DEAD_ZONE) return false;     // e.g. x = 0.1, y = 0.1: length 0.14 < 0.30, not aiming
    if (y < 0) {                                        // pointing below the horizon: there is only ground down there,
        if (std::fabs(x) < DEAD_ZONE) return false;     // so straight down means "not aiming" (the last angle stays),
        degrees = (x > 0) ? 0.0f : 180.0f;              // and down-right / down-left snap to a flat throw to that side
        return true;
    }
    degrees = std::atan2(y, x) * 180.0f / 3.14159265f;  // 0 = right, 90 = up (atan2(1, 0)), 180 = left
    return true;
}

Action ActionFromEvent(const SDL_Event& event) {
    if (event.type != SDL_CONTROLLERBUTTONDOWN) return NoAction;
    if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) return ThrowAction;    // X (cross)
    if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) return RecallAction;   // O (circle)
    return NoAction;
}

}
