#include "MoveAction.h"
#include "Object.h"


using namespace Supernova;

MoveAction::MoveAction(Vector3 endPosition, float duration, bool loop): TimeAction(duration, loop){
    this->endPosition = endPosition;
    this->objectStartPosition = true;
}

MoveAction::MoveAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop): TimeAction(duration, loop){
    this->startPosition = startPosition;
    this->endPosition = endPosition;
    this->objectStartPosition = false;
}

MoveAction::~MoveAction(){
    
}

bool MoveAction::run(){
    if (!TimeAction::run())
        return false;
    
    if (object && objectStartPosition){
        startPosition = object->getPosition();
    }
    
    return true;
}

bool MoveAction::step(){
    if (!TimeAction::step())
        return false;
    
    if (object){
        Vector3 move = (endPosition - startPosition) * value;
        object->setPosition(startPosition + move);
    }
    
    return true;
}
