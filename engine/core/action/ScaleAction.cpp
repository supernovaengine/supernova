#include "ScaleAction.h"

#include "Object.h"


using namespace Supernova;

ScaleAction::ScaleAction(Vector3 endScale, float duration, bool loop): TimeAction(duration, loop){
    this->endScale = endScale;
    this->objectStartScale = true;
}

ScaleAction::ScaleAction(Vector3 startScale, Vector3 endScale, float duration, bool loop): TimeAction(duration, loop){
    this->startScale = startScale;
    this->endScale = endScale;
    this->objectStartScale = false;
}

ScaleAction::~ScaleAction(){

}

bool ScaleAction::run(){
    if (!TimeAction::run())
        return false;

    if (object && objectStartScale){
        startScale = object->getScale();
    }

    return true;
}

bool ScaleAction::step(){
    if (!TimeAction::step())
        return false;

    if (object){
        Vector3 scale = (endScale - startScale) * value;
        object->setScale(startScale + scale);
    }

    return true;
}
