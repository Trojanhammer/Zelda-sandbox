#include "PhysicsObject.h"
#include <cmath>

void PhysicsObject::Update(float deltaTime){
    float friction = 0.7f;
    posX += vX * deltaTime;
    vY += gravity * deltaTime; // because gravity is acceleratin (it is force that always pull object)
    if(vY >=-10.0f && vY < 0.0f){
        vY=0.0f;
    }
    posY += vY * deltaTime;
    if (posY >= 450.0f){
        posY = 450.0f;
        vY =0.0f;
        vX = pow(friction, deltaTime * 60) * vX; // vX will continue berguling
        if(std::abs(vX) <=10.0f){ // handles throwing to left side which is (negative vX too)
            vX =0;
        }
    }
    if((vX ==0) && (vY ==0)){
        //state = Idle;
    }

}


