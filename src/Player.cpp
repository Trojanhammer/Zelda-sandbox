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