#include "Monster.h"
#include <cmath>

Monster::Monster(){
    baseposX = 700;
    baseposY = 426;
    posX = 700;
    posY = 426;
    groundY = 426;
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

bool Monster::Detect_danger(Player& player){
    
    if(player.weapon.state == Weapon::Idle && player.weapon.posY == player.weapon.groundY){
        float distance;
        distance = std::abs(player.weapon.posX - posX);
        isLeft = player.weapon.posX < posX;
        vX = 20; // set speed of patrol
        if(isLeft){
            vX = -vX;
        }

        if(distance < 100){
            state = Monster::Patrol;
            return true;
        }
        return false;
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


void Monster::Update_monster(float deltaTime, Player& player){
    switch(state){
        case Monster::Idle :
            if(Detect_player(player)) state = Monster::Chasing;
            else if(Detect_danger(player)) state = Monster::Patrol;
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