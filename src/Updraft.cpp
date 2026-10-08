#include "Updraft.h"
#include <cmath>

void Updraft::Update(float deltaTime){
    max_duration -= deltaTime;
    if(max_duration <=0){
        if(!decayStarted){
            updraft_force = gravity_force;
            decayStarted = true;
        }
        updraft_force *= std::pow(0.8, deltaTime); 
        netforce = updraft_force - gravity_force;
        vY -= (netforce * deltaTime) / player -> mass * 100; // 1 meter is 100px.bcs without * 100, it is in m/s not px/s
        player -> posY += vY * deltaTime;

        if (player -> posY >= 406.0f){
            player -> posY = 406.0f;
            vY =0;
            return;
        }
        return;
    }
    updraft_force += accumulationRate * deltaTime;
    if(updraft_force >= maxUpdraftForce && max_duration > 0){
        updraft_force = maxUpdraftForce;
        vY = 0;
        player -> posY += vY * deltaTime;
        return;
    }
    netforce = updraft_force - gravity_force;
    if(netforce > 0){
        vY -= (netforce * deltaTime) / 70 * 100; // 70 is mass
        player -> posY += vY * deltaTime;
    }
}