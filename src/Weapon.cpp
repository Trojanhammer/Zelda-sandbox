#include "Weapon.h"
#include <cmath>
#include <algorithm>

Weapon::Weapon() {
    posX = baseposX;
    posY = baseposY;
}

void Weapon::Recall(float deltaTime,float PlayerposX){ // TOTK Recall Mechanic 
    // (1,1.6,1.5) -> (2,3.2,3) -> (2.7,2.3,3.5)
        // unique(start , end, comparison rule) , will give pointer when it starts to duplicate
        // remove duplicates

        RecallCoord.erase(std::unique(RecallCoord.begin(), RecallCoord.end(), [](const auto& a, const auto& b){
        return (a.posX == b.posX) && (a.posY == b.posY);}), RecallCoord.end());
        RecallClock = std::min(RecallClock,RecallCoord.back().time);
        RecallClock -= deltaTime;
        int i;
        for (i = RecallCoord.size() -1 ; i>0; i--){
            if (RecallCoord[i].time == RecallClock){
                posX = RecallCoord[i].posX;
                posY = RecallCoord[i].posY;
                break;
            }
            else if (RecallCoord[i].time > RecallClock && RecallCoord[i-1].time < RecallClock){ 
                // Linear Interpolation(lerp) between two points to get the position of X and Y
                const RecallPoint& PointBefore = RecallCoord[i-1];
                const RecallPoint& PointAfter = RecallCoord[i];
                float ratio = (RecallClock - PointBefore.time)/(PointAfter.time - PointBefore.time);
                posX = PointBefore.posX + (ratio * (PointAfter.posX - PointBefore.posX));
                posY = PointBefore.posY + (ratio * (PointAfter.posY - PointBefore.posY));
                break; 
            }

        }
        if(RecallClock <= RecallCoord[0].time){
            RecallClock=0.0f;
            posX = RecallCoord[0].posX;
            posY  = RecallCoord[0].posY;
            RecallCoord.clear();
            vX =0;
            vY=0;
            if(posX != PlayerposX + 22){
                state = Weapon::Fall;
            }
            else{
                state = Weapon::Held;
                return;
            }
        }
        
    }

void Weapon::Fallen(float deltaTime){
    RecallCoord.push_back({posX,posY,RecallClock});
    vY += gravity * deltaTime;
    posY += vY * deltaTime;
    if (posY >= 450.0f){ // middle of swords stop here indicating it reaches the grass
        posY = 450.0f;
        vY =0.0f;
    }
    if((vX ==0) && (vY ==0)){
        state = Weapon::Idle;
    }
    RecallClock +=deltaTime;
}




