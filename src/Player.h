#pragma once
#include "Weapon.h"

class Player{
    public:
        float posX = 100;
        float posY = 100;
        float throwing_speed = 800; // 1000px/s
        std::string name = "Link";
        Weapon weapon; // store weapon object inside player obj instead of ptr since no need to do so
    public:
        Player(Weapon weapon) : weapon(weapon){
        }
        void Update(float deltaTime);
        void Throw();
};