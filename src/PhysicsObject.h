#pragma once

class PhysicsObject {
    public:
        float posX = 0;
        float posY = 0;
        float groundY;
        float baseposX = 0;
        float baseposY = 0;  
        float vX =0;
        float vY =0;
        float lift = 0; // acceleration that oppose gravity (px/s square)
        static constexpr float gravity = 980.0f; // 98px per second square

    public:
        void Update(float deltaTime);
        //void Fallen(float deltaTime);
        void OpposeGravity(float deltaTime, float groundY);
};