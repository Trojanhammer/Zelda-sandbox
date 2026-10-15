#pragma once
#include "PhysicsObject.h"
#include "Recall.h"

class Box : public PhysicsObject { // ensure public and procted from parent stay the same visibility to the child ones.
    public:
        bool isExist = true;
        enum State {
            Idle,
            Magnesis,
            Fly,
            Recalling,
            Stasis
        };
        State state = Idle;
        Recall recall;
    public:
        Box(){
            groundY = 410;
            posX = 400;
            posY = 410;
        }
        void UpdateBox(float deltaTime);
};