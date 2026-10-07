#include "Player.h"
#include <cmath>

void Player::Throw(float angleDegrees){
    float const PI = 3.14159265f;
    weapon.state = Weapon::Thrown;

    float angleRadian = angleDegrees * (PI/180.0f);

    // determine initial vX and vY
    weapon.vX = throwing_speed * cos(angleRadian); // cos(a) = adjacent/hypotenous
    weapon.vY = -throwing_speed * sin(angleRadian);
}

void Player::Walking(float deltaTime){ // Ensure both Link and Weapon is moving
    float pickuprange = 50;
    posX += RightWalk? walking_speed * deltaTime : -walking_speed * deltaTime;
    if(weapon.state == Weapon::Held){
        weapon.posX += RightWalk? walking_speed * deltaTime : -walking_speed * deltaTime;
    }
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