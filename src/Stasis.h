#pragma once
#include "PhysicsObject.h"
#include "Player.h"
#include <cmath>

class Stasis {
    public:
        float timeLimit = 4.0f ; // 4 seconds
        float time_record = 0.0f;
        static constexpr float velocity = 50.0f ; // the n-th hit adds n * 50 px/s (1st: 50, 2nd: 100, 3rd: 150, ...)
        int hit_frequency = 0;
    public:
        bool UpdateStasis(bool hit,float deltaTime,float angleDegrees,PhysicsObject& obj){ // this fx will invoke on how many times it is been hit
            time_record += deltaTime;
            if(time_record >= timeLimit){
                return true;
            }
            if(hit){
                hit_frequency += 1;
                float const PI = 3.14159265f;
                float angleRadian = angleDegrees * (PI/180.0f);
                obj.vX += hit_frequency * velocity * cos(angleRadian);
                obj.vY -= hit_frequency * velocity * sin(angleRadian);
            }
            return false;
        }
};