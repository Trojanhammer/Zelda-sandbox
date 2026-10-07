#pragma once
#include <string>
#include <vector>
#include <utility>

class Weapon{
    public:
        std::string name;
        float baseposX =122;
        float baseposY =394;   // Link's hand: his middle (100, 406) + (22, -12) = (122, 394)
        float posX = baseposX;
        float posY = baseposY;
        float vX =0;
        float vY =0;
        float RecallLimit = 3.0f;
        float RecallClock =0.0f;
        float gravity = 980.0f; // 98px per second square
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
        Weapon(std::string name);
        void Recall(float deltaTime,float PlayerposX);
        void Update(float deltaTime);
        void Fallen(float deltaTime);
        
};