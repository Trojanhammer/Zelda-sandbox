#include "Player.h"
#include <cmath>

void Player::Update(float deltaTime){
    if(weapon.state == Weapon::Thrown){
        // assume speed throwing is 980px/s2
        float gravity = 980.0f; // 98px per second square
        float friction = 0.7f;
        weapon.posX += weapon.vX * deltaTime;
        weapon.vY += gravity * deltaTime; // because gravity is acceleratin (it is force that always pull object)
        if(weapon.vY >=-10.0f && weapon.vY < 0.0f){
            weapon.vY=0.0f;
        }
        weapon.posY += weapon.vY * deltaTime;
        if (weapon.posY >= 300.0f){ // Check has arrived at grass or not
            weapon.posY = 300.0f;
            weapon.vY =0.0f;
            weapon.vX = pow(friction, deltaTime * 60) * weapon.vX; // vX will continue berguling
            if(std::abs(weapon.vX) <=10.0f){ // handles throwing to left side which is (negative vX too)
                weapon.vX =0;
            }
        }
        if((weapon.vX ==0) && (weapon.vY ==0)){
        weapon.state = Weapon::Idle;
        }
    }
    
}

void Player::Throw(){
    float angle = 30; // 30 degree
    float const PI = 3.14159265f;
    weapon.state = Weapon::Thrown;

    float angleRadian = angle * (PI/180.0f);

    // determine initial vX and vY
    weapon.vX = throwing_speed * cos(angleRadian); // cos(a) = adjacent/hypotenous
    weapon.vY = -throwing_speed * sin(angleRadian);
}