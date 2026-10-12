#pragma once
#include "PhysicsObject.h"

class Box : public PhysicsObject { // ensure public and procted from parent stay the same visibility to the child ones.
    public:
        bool isExist = true;
        enum State {
            Idle,
            Fly
        };
        State state = Idle;
    public:
        Box(){
            groundY = 410;
            posX = 400;
            posY = 410;
        }

};