#include "RotateAction.h"

#include "Object.h"

using namespace Supernova;

RotateAction::RotateAction(Quaternion endRotation, float duration, bool loop){
    this->endRotation = endRotation;
    this->objectStartRotation = true;
}

RotateAction::RotateAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop){
    this->startRotation = startRotation;
    this->endRotation = endRotation;
    this->objectStartRotation = false;
}

RotateAction::~RotateAction(){

}

bool RotateAction::run(){
    if (!TimeAction::run())
        return false;

    if (object && objectStartRotation){
        startRotation = object->getRotation();
    }

    return true;
}

bool RotateAction::step(){
    
    object->setRotation(startRotation);
    
    /*
    if (!TimeAction::step())
        return false;

    if (object){
        Quaternion move = (endRotation - startRotation) * value;
        object->setRotation(startRotation + move);
    }
*/
    return true;
}
