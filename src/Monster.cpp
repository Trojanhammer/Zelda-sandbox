#include "Monster.h"
#include <cmath>

Monster::Monster(){
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

void Monster::Update_monster(float deltaTime, Player& player){
    switch(state){
        case Monster::Idle :
            if(Detect_player(player)) state = Monster::Chasing;
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

    }
}