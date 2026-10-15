#pragma once
#include "Updraft.h"
#include "PhysicsObject.h"
#include "Player.h"
#include <memory>


class Fire : public PhysicsObject{
    public:
        float diameter = 15; // 15 px
        std::unique_ptr<Updraft> updraft = nullptr;
        bool isAlive = true;
        bool isUpDraft = false;
        enum State {
            Start_Throw,
            Thrown,
            Firing
        };
        State state = Start_Throw;
    public:
        Fire(){
            groundY = 450;
        }
        void UpdateFire(float deltaTime,float throwAngle, Player& player);
};