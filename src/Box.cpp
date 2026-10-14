#include "Box.h"

void Box::UpdateBox(float deltaTime){
    switch(state){
        case Box::Idle :
            recall.RecallWhenIdle(deltaTime,*this); // *this means its object
            break;

        case Box::Magnesis :
            UpdatePosition(deltaTime);
            break;

        case Box::Fly :
            recall.Record(posX,posY);
            UpdatePosition(deltaTime);
            recall.UpdateRecallClock(deltaTime);
            if (vX ==0 && vY ==0){
                state = Box::Idle;
            }
            break;

        case Box::Recalling :
            if(recall.Rewind(deltaTime,*this)) state = Box::Fly;
            break;
    };
}