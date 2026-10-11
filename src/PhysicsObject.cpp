#include "PhysicsObject.h"
#include <cmath>

void PhysicsObject::Update(float deltaTime){ // Update position of object when no updraft.Updraft has its own update() which has continous force upwards unlike throwing which is one time force and decrease by gravity.
    float friction = 0.7f;
    posX += vX * deltaTime;
    OpposeGravity(deltaTime);
    if (posY >= groundY){
        vX = pow(friction,deltaTime * 60) * vX; // vX will continue berguling
        if(std::abs(vX) <=10.0f){ // handles throwing to left side which is (negative vX too)
            vX =0;
        }
    }
}

void PhysicsObject::OpposeGravity(float deltaTime){ // groundY is y axis when on ground depends on each object
    float diff = gravity - lift;
    vY += diff * deltaTime;
    posY += vY * deltaTime;
    if(posY >= groundY){ // check if already past grass or not
        posY = groundY;
        vY = 0;
        lift = 0;
        return;
    }
    lift = 0; // reset again lift
}


