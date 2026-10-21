#pragma once
#include "PhysicsObject.h"
#include "Player.h"

class Monster : public PhysicsObject{
    public:
        float health = 100;
        float max_health = 100;
        float attack_power = 20;
        float distance = 0 ; // distance from enemy to player
        float step = 0.0f; // distance on how far enemy has chase
        static constexpr float distance_attack = 60; // enemy can only attack when in 20px from player
        bool isLeft = true; // if player is on left of enemy
        float time = 0.0f; // count time for attack
        float patrol_limit = 3.0f; // 3 seconds limit time for patrol
        float patrol_time_count = 0.0f;
        std::vector<PhysicsObject*> heard; // store new pointer to the object it reacts
        enum State {
            Idle,
            Patrol,
            Chasing,
            BackBase,
            Attack
        };
        State state = Monster::Idle;
    public:
        Monster();
        bool Detect_player(Player& player);
        bool Detect_danger(PhysicsObject& obj); // detect if player tries to attack from afar using weapon
        void Patrolling(float deltaTime);
        void Chase_player(float deltaTime,Player& player);
        void Attack_player(float deltaTime,Player& player);
        void Back_base(float deltaTime);
        void Update_monster(float deltaTime,std::vector<PhysicsObject*>& throwables,Player& player); // send vector of throwables(objects in motion basically) that exist in each frame
        // std::vector<PhysicsObject*> = this vector stores pointer . & throwables means we pass by reference since a vector is a object by default
        bool At_Rest(PhysicsObject& obj);
};