#include "MoveAction.h"
#include "Object.h"


using namespace Supernova;

MoveAction::MoveAction(Vector3 endPosition, float duration, bool loop): Action(duration, loop){
    this->endPosition = endPosition;
    this->objectStartPosition = true;
}

MoveAction::MoveAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop): Action(duration, loop){
    this->startPosition = startPosition;
    this->endPosition = endPosition;
    this->objectStartPosition = false;
}

MoveAction::~MoveAction(){
    
}

void MoveAction::play(){
    Action::play();
    
    if (object && objectStartPosition){
        startPosition = object->getPosition();
    }
}

void MoveAction::step(){
    Action::step();
    
    if (object){
        Vector3 move = (endPosition - startPosition) * value;
        object->setPosition(startPosition + move);
    }
}
