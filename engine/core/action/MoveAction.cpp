#include "MoveAction.h"
#include "Object.h"


using namespace Supernova;

MoveAction::MoveAction(Vector3 position, float duration, bool loop): Action(duration, loop){
    this->position = position;
}

MoveAction::~MoveAction(){
    
}

void MoveAction::start(){
    Action::start();
    
    if (object){
        startPosition = object->getPosition();
    }
}

void MoveAction::step(){
    Action::step();
    
    if (object){
        Vector3 move = (position - startPosition) * value;
        object->setPosition(startPosition + move);
    }
}
