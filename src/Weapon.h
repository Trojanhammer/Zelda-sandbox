#pragma once
#include <string>
#include <vector>
#include <utility>

#include "PhysicsObject.h"
#include "Recall.h"

class Weapon : public PhysicsObject{
    public:
        std::string name = "Master Sword";
        float baseposX =122;
        float baseposY =394;   // Link's hand: his middle (100, 406) + (22, -12) = (122, 394)
        enum State
        {
            Start_Throw,
            Fall,
            Held,
            Thrown,
            Idle,
            Recalling
        };
        State state = Held;
        Recall recall; // store recall obj in weapon

    public:
        Weapon();
};