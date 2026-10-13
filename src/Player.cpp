#include "Player.h"
#include <cmath>

Player::Player(Weapon weapon) : weapon(weapon){
    posX = 100;
    posY = 406; // 462 (the ground surface) - 56 (half of Link's 112 px): his feet touch the ground
    groundY = 406;
}

void Player::Throw(float angleDegrees,PhysicsObject& obj){
    float const PI = 3.14159265f;
    float angleRadian = angleDegrees * (PI/180.0f);

    // determine initial vX and vY
    obj.vX = throwing_speed * cos(angleRadian); // cos(a) = adjacent/hypotenous
    obj.vY = -throwing_speed * sin(angleRadian);
}

void Player::Walking(float deltaTime){
    float pickuprange = 50;
    posX += RightWalk? walking_speed * deltaTime : -walking_speed * deltaTime;
    if((abs(posX-weapon.posX) < pickuprange) && (weapon.state == Weapon::Idle)){
        Pickup(HandX, HandY);
    }
}
 
void Player::Pickup(float handX, float handY){ // Pickup will reset for recall
    weapon.posX = posX + handX;
    weapon.posY = posY + handY;
    weapon.state = Weapon::Held;
    weapon.RecallCoord.clear();
    weapon.RecallClock = 0.0f;
}

void Player::UpdateWeapon(float deltaTime, float throwAngle){
    switch(weapon.state){
        case Weapon::Start_Throw : 
            Throw(throwAngle,weapon);
            weapon.state = Weapon::Thrown;
            break;
        
        case Weapon::Thrown :
            weapon.RecallCoord.push_back({weapon.posX,weapon.posY,weapon.RecallClock});
            weapon.UpdatePosition(deltaTime);
            if((weapon.vX ==0) && (weapon.vY ==0)){
                weapon.state = Weapon::Idle;
            }
            weapon.RecallClock +=deltaTime;
            break;
        
        case Weapon::Recalling :
            weapon.Recall(deltaTime,posX);
            break;
        
        case Weapon::Fall :
            weapon.RecallCoord.push_back({weapon.posX,weapon.posY,weapon.RecallClock});
            weapon.UpdatePosition(deltaTime);
            if((weapon.vX ==0) && (weapon.vY ==0)){
                weapon.state = Weapon::Idle;
            }
            weapon.RecallClock +=deltaTime;
            break;
        case Weapon::Held :
            weapon.posX = HandX + posX;
            weapon.posY = HandY + posY;
            break;

        case Weapon::Idle :
            if((weapon.RecallClock < weapon.RecallLimit) &&  weapon.RecallClock > 0.0f){
                weapon.RecallClock += deltaTime;
                weapon.RecallCoord.push_back({weapon.posX,weapon.posY,weapon.RecallClock});
            }
            else if(weapon.RecallClock >= weapon.RecallLimit){
                weapon.RecallCoord.clear();
                weapon.RecallClock = 0.0f;
            }
            break;
        }
}