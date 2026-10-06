#pragma once
#include "Weapon.h"

class Player{
    public:
        float posX = 100;
        float posY = 406;   // 462 (the ground surface) - 56 (half of Link's 112 px): his feet touch the ground
        float throwing_speed = 800; // 1000px/s
        std::string name = "Link";
        Weapon weapon; // store weapon object inside player obj instead of ptr since no need to do so
    public:
        Player(Weapon weapon) : weapon(weapon){
        }
        void Throw(float angleDegrees);
};