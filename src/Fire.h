#pragma once
#include "Updraft.h"
#include "PhysicsObject.h"

class Fire : public PhysicsObject{
    public:
        float diameter = 15; // 15 px
        Updraft* updraft = nullptr;
        bool isAlive = false;
        bool isUpDraft = false;
        enum State {
            Start_Throw,
            Thrown,
            Firing
        };
        State state = Start_Throw;
};