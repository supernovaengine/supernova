#include "RotateAction.h"

#include "Object.h"
#include "platform/Log.h"

using namespace Supernova;

RotateAction::RotateAction(Quaternion endRotation, float duration, bool loop): TimeAction(duration, loop){
    this->endRotation = endRotation;
    this->objectStartRotation = true;
    this->shortestPath = false;
}

RotateAction::RotateAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop): TimeAction(duration, loop){
    this->startRotation = startRotation;
    this->endRotation = endRotation;
    this->objectStartRotation = false;
    this->shortestPath = false;
}

RotateAction::RotateAction(float endAngle2D, float duration, bool loop): TimeAction(duration, loop){
    Quaternion endRotation;
    endRotation.fromAngle(endAngle2D);

    this->endRotation = endRotation;
    this->objectStartRotation = true;
    this->shortestPath = false;
}

RotateAction::RotateAction(float startAngle2D, float endAngle2D, float duration, bool loop): TimeAction(duration, loop){
    Quaternion startRotation;
    startRotation.fromAngle(startAngle2D);

    Quaternion endRotation;
    endRotation.fromAngle(endAngle2D);

    this->startRotation = startRotation;
    this->endRotation = endRotation;
    this->objectStartRotation = false;
    this->shortestPath = false;
}

RotateAction::~RotateAction(){

}

bool RotateAction::isShortestPath(){
    return shortestPath;
}

void RotateAction::setShortestPath(bool shortestPath){
    this->shortestPath = shortestPath;
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
    if (!TimeAction::step())
        return false;

    if (object) {
        object->setRotation(object->getRotation().slerp(value, startRotation, endRotation, shortestPath));
    }

    return true;
}
