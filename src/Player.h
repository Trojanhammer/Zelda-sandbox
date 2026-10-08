#pragma once
#include "Weapon.h"
#include "PhysicsObject.h"

class Player : public PhysicsObject {
    public:
        float throwing_speed = 800; // 800px/s
        float walking_speed = 150;
        static constexpr float mass = 70.0f;
        static constexpr float HandX = 22.0f; // distance between link pos to its hand(sword)
        static constexpr float HandY = -12.0f;
        enum State {
            Idle,
            Walk
        };
        State state = Idle;
        bool RightWalk = true; 
        std::string name = "Link";
        Weapon weapon; // store weapon object inside player obj instead of ptr since no need to do so
    public:
        Player(Weapon weapon);
        void Throw(float angleDegrees, PhysicsObject& obj);
        void Walking(float deltaTime);
        void Pickup(float handX, float handY);
};