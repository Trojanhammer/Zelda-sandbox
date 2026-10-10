#pragma once
#include <string>
#include <vector>
#include <utility>

#include "PhysicsObject.h"

class Weapon : public PhysicsObject{
    public:
        std::string name = "Master Sword";
        float baseposX =122;
        float baseposY =394;   // Link's hand: his middle (100, 406) + (22, -12) = (122, 394)
        float RecallLimit = 3.0f;
        float RecallClock =0.0f;
        struct RecallPoint{
            float posX;
            float posY;
            float time;
        };
        std::vector<RecallPoint> RecallCoord;
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
    public:
        Weapon();
        void Recall(float deltaTime,float PlayerposX);
        void Fallen(float deltaTime);
        
};