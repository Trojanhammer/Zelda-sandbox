#include "Recall.h"

void Recall::Record(float posX, float posY){ // Record position of any of the moving related entity
    RecallCoord.push_back({posX,posY,RecallClock});
}

bool Recall::Rewind(float deltaTime, PhysicsObject& obj){ // send true or false to indicate whether rewind completed or not
    if(StartRecall == true){
        RecallCoord.erase(std::unique(RecallCoord.begin(), RecallCoord.end(), [](const auto& a, const auto& b){
        return (a.posX == b.posX) && (a.posY == b.posY);}), RecallCoord.end());
        RecallClock = std::min(RecallClock,RecallCoord.back().time);
    }
    RecallClock -= deltaTime;
    int i;
    for (i = RecallCoord.size() -1 ; i>0; i--){
        if (RecallCoord[i].time == RecallClock){
            obj.posX = RecallCoord[i].posX;
            obj.posY = RecallCoord[i].posY;
            break;
        }
        else if (RecallCoord[i].time > RecallClock && RecallCoord[i-1].time < RecallClock){ 
            // Linear Interpolation(lerp) between two points to get the position of X and Y
            const RecallPoint& PointBefore = RecallCoord[i-1];
            const RecallPoint& PointAfter = RecallCoord[i];
            float ratio = (RecallClock - PointBefore.time)/(PointAfter.time - PointBefore.time);
            obj.posX = PointBefore.posX + (ratio * (PointAfter.posX - PointBefore.posX));
            obj.posY = PointBefore.posY + (ratio * (PointAfter.posY - PointBefore.posY));
            break; 
            }

    }
    StartRecall = false;
    if(RecallClock <= RecallCoord[0].time){
        Clear(obj);
        return true;
    }
    return false;
}

void Recall::Clear(PhysicsObject& obj){
    RecallClock=0.0f;
    obj.posX = RecallCoord[0].posX;
    obj.posY  = RecallCoord[0].posY;
    RecallCoord.clear();
    obj.vX =0;
    obj.vY=0;
    StartRecall = true;
}

