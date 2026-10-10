#include "PhysicsObject.h"
#include <cmath>

void PhysicsObject::Update(float deltaTime){
    float friction = 0.7f;
    posX += vX * deltaTime;
    vY += gravity * deltaTime; // because gravity is acceleratin (it is force that always pull object)
    if(vY >=-5.0f && vY < 0.0f){
        vY=0.0f;
    }
    posY += vY * deltaTime;
    if (posY >= 450.0f){
        posY = 450.0f;
        vY =0;
        vX = pow(friction,deltaTime * 60) * vX; // vX will continue berguling
        if(std::abs(vX) <=10.0f){ // handles throwing to left side which is (negative vX too)
            vX =0;
        }
    }
    // if((vX ==0) && (vY ==0)){
    //    state = Idle;
    // }
}

void PhysicsObject::OpposeGravity(float deltaTime, float groundY){ // groundY is y axis when on ground depends on each object
    float diff = gravity - lift;
    vY += diff * deltaTime;
    posY += vY * deltaTime;
    if(posY >= groundY){ // check if already past grass or not
        posY = groundY;
        vY = 0;
        return;
    }
    lift = 0; // reset again lift

}


