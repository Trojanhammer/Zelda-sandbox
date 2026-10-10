#include "Updraft.h"
#include "Player.h"
#include <cmath>

void Updraft::Update(float deltaTime){
    max_duration -= deltaTime;
    if(max_duration <=0){ // Start Decayed
        if(!decayStarted){
            updraft_force = gravity_force;
            decayStarted = true;
        }
        updraft_force *= std::pow(0.8, deltaTime); 
        //netforce = updraft_force - gravity_force; // netforce will be negative here
        if(updraft_force <= 50){
            updraft_force =0;
            vY =0;
            isFinished = true;
            return;
        }
        return;
    }
    updraft_force += accumulationRate * deltaTime;
    if(updraft_force >= maxUpdraftForce && max_duration > 0){ // Set limit so player will floating on the air
        updraft_force = maxUpdraftForce;
        vY = 0;
        return;
    }
    netforce = updraft_force - gravity_force;
}

void Updraft::Push(float deltaTime, PhysicsObject& obj){
    if(max_duration <= 0){
        //obj.posY += vY * deltaTime;

        if (obj.posY >= 406.0f){
            obj.posY = 406.0f;
            vY =0;
            isFinished = true;
            return;
        }
    }
    if(updraft_force >= maxUpdraftForce && max_duration > 0){ // Set limit so player will floating on the air
        obj.lift = PhysicsObject::gravity;
        return;
    }
    obj.lift = updraft_force / Player::mass * 100;
}