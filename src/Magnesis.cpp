#include "Magnesis.h"
#include <cmath>

void Magnesis::Update(PhysicsObject& obj, float deltaTime, float moveAngle){ // Determine vX and vY only
    float const PI = 3.14159265f;
    float angleRadian = moveAngle * (PI/180.0f);
    obj.vX = magnesis_speed * cos(angleRadian);
    obj.vY = magnesis_speed * sin(angleRadian);
    obj.lift = PhysicsObject::gravity;
}