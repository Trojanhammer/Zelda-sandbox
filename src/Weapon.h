#pragma once
#include <string>

class Weapon{
    public:
        std::string name;
        float posX;
        float posY;
        float vX =0;
        float vY =0;
        enum State
        {
            Held,
            Thrown,
            Idle,
            Recall
        };
        State state = Held;
    public:
        Weapon(std::string name,float posX, float posY);
        
};