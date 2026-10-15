#include "Fire.h"

void Fire::UpdateFire(float deltaTime,float throwAngle,Player& player){
    switch(state) {
        case Fire::Start_Throw :
            posX = player.posX + Player::HandX;
            posY = player.posY + Player::HandY;
            player.Throw(throwAngle,*this);
            state = Fire::Thrown; 
            break;
        case Fire::Thrown :
            UpdatePosition(deltaTime);
            if(vX ==0 && vY ==0){
                state = Fire::Firing;
                if(!isUpDraft){
                updraft = std::make_unique<Updraft>();             
                }
            }
            break;
        case Fire::Firing :
            updraft -> Update(deltaTime);
            if(std::abs(posX - player.posX) <= 10){
                updraft -> Push(deltaTime,player);
            }
            if(updraft -> isFinished){
                isAlive = false;
                updraft.reset();
                    // delete both fire object and pointer here/
            }
            break;
            }
}