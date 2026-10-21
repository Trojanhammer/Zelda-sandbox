#include "Monster.h"
#include <cmath>
#include <algorithm>

Monster::Monster(){
    baseposX = 700;
    baseposY = 426;
    posX = 700;
    posY = 426;
    groundY = 426;
}

bool Monster::At_Rest(PhysicsObject& obj){ // check obj idle or not
    if(obj.vX ==0 && obj.vY ==0 && obj.posY >= obj.groundY) return true;
    else return false;
}

void Monster::Chase_player(float deltaTime,Player& player){
    isLeft = player.posX < posX;
    vX = 60; // assume same speed of chasing
    if(isLeft){
        vX = -vX;
    }
    posX += vX * deltaTime;
    step += std::abs(vX * deltaTime);
    if(step >= 300.0){ // enemy will stop chase once has travel 300px
        state = Monster::BackBase;
        step = 0.0f;
    }

    // monster will attack one time per second as long as enemy is near to player
    distance = std::abs(posX - player.posX);
    
    if (distance < distance_attack){
        state = Monster::Attack;
    }
}

bool Monster::Detect_player(Player& player){
    distance = std::abs(posX - player.posX);
    if(distance <= 100){
        state = Monster::Chasing;
        return true;
    }
    return false;
}

void Monster::Attack_player(float deltaTime,Player& player){ // run on every second when available to attack
    time += deltaTime;
    if(time >= 1.0f) {
        player.health -= attack_power;
        time = 0.0f;
    }
    else{
        return;
    }
}

bool Monster::Detect_danger(PhysicsObject& obj){
    if(std::find(heard.begin(), heard.end(), &obj) != heard.end()) return false; // if the obj is in note, then ignore
    float weapon_distance;
    weapon_distance = std::abs(obj.posX - posX);
    if(At_Rest(obj) && weapon_distance < 100){
        isLeft = obj.posX < posX;
        vX = 20; // set speed of patrol
        if(isLeft){
            vX = -vX; 
        }
        heard.push_back(&obj); // store new pointer
        state = Monster::Patrol;
        return true;
    }
    else
        return false;
}

void Monster::Patrolling(float deltaTime){
    patrol_time_count += deltaTime;
    if(patrol_time_count >= patrol_limit){
        state = Monster::BackBase;
        patrol_time_count = 0.0f;
    }
    posX += vX * deltaTime;
}

void Monster::Back_base(float deltaTime){
    vX = 60; //assume same speed when running
    if(!isLeft){
        vX = -vX;
    }
    posX += vX * deltaTime;
    if((vX > 0 && posX >= baseposX) || (vX < 0 && posX <= baseposX)){
        // check from both side when back to base(back from right or from left)
        posX = baseposX;
        state = Monster::Idle;
    }
}


void Monster::Update_monster(float deltaTime, std::vector<PhysicsObject*> &throwables,Player& player){
    //
    switch(state){
        case Monster::Idle :
            if(Detect_player(player)) state = Monster::Chasing;
            else{
                for(PhysicsObject* thing : throwables){ // make another pointer that points towards a reference vector
                    if(Detect_danger(*thing)) {
                        state = Monster::Patrol;
                        break;
                    }
                }
            }
            break;
        
        case Monster::Chasing :
            Chase_player(deltaTime,player);
            break; 
        
        case Monster::Attack :
            distance = std::abs(posX - player.posX);
    
            if (distance < distance_attack){
                Attack_player(deltaTime,player);
            }
            else{
                state = Monster::Chasing;
                Chase_player(deltaTime,player);
            }
            break;
        
        case Monster::Patrol :
            Patrolling(deltaTime);
            if(Detect_player(player)) state = Monster::Chasing;
            break;
        
        case Monster::BackBase :
            Back_base(deltaTime);
            break;
    }
}