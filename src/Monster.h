#pragma once
#include "PhysicsObject.h"
#include "Player.h"

class Monster : public PhysicsObject{
    public:
        float health = 100;
        float max_health = 100;
        float attack_power = 20;
        float distance = 0 ; // distance from enemy to player
        static constexpr float distance_attack = 60; // enemy can only attack when in 20px from player
        bool isLeft = true; // if player is on left of enemy
        float time = 0.0f; // count time for attack
        enum State {
            Idle,
            Chasing,
            Attack
        };
        State state = Monster::Idle;
    public:
        Monster();
        bool Detect_player(Player& player);
        void Chase_player(float deltaTime,Player& player);
        void Attack_player(float deltaTime,Player& player);
        void Update_monster(float deltaTime,Player& player);
};