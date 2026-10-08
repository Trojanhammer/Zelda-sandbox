#pragma once

class PhysicsObject {
    public:
        float posX = 0;
        float posY = 0;
        float baseposX = 0;
        float baseposY = 0;  
        float vX =0;
        float vY =0;
        static constexpr float gravity = 980.0f; // 98px per second square

    public:
        void Update(float deltaTime);
        //void Fallen(float deltaTime);
};