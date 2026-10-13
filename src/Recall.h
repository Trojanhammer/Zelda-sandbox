#pragma once
#include "PhysicsObject.h"
#include <vector>
#include <algorithm>

class Recall {
    public:
        float RecallLimit = 3.0f;
        float RecallClock = 0.0f;
        bool StartRecall = true;
        struct RecallPoint{
            float posX;
            float posY;
            float time;
        };
        std::vector<RecallPoint> RecallCoord;
    
    public:
        void Record(float posX, float posY);
        bool Rewind(float deltaTime,PhysicsObject& obj);
        void Clear(PhysicsObject& obj);
};