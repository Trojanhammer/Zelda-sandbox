#pragma once
#include "Box.h"
// only metal can be magnesis or can be called as ultrahand but limit that ultrahand cant combine item
class Magnesis {
    public:
        static constexpr float gravity = 980.0f;
        static constexpr float gravity_force = 9.8 * 10; // assume mass is 10 for all
        bool isFinished = false;
        static constexpr float magnesis_speed = 100; // 100px/s
    public:
        void Update(PhysicsObject& obj, float deltaTime,float moveAngle);
    
};