#pragma once
#include "PhysicsObject.h"

class Updraft {
    public:
        static constexpr float gravity = 9.8f; //constexpr is const experession that have const values and its calculation is done while compiling , not in everyf frame
        static constexpr float gravity_force = 9.8 * 70;
        static constexpr float accumulationRate = 300.0f; // rate for fire.force per second
        static constexpr float maxUpdraftForce = 1050.0f;
        bool decayStarted = false;
        bool isFinished = false;
        float max_duration = 10.0f;
        float vY = 0.0f;
        float updraft_force =0.0f;
        float netforce = 0.0f;

    public:
        void Update(float deltaTime);
        void Push (float deltaTime, PhysicsObject& obj);
};