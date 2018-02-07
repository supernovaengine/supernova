#include "AlphaAction.h"

#include "Object.h"
#include "ConcreteObject.h"

using namespace Supernova;

AlphaAction::AlphaAction(float endAlpha, float duration, bool loop): TimeAction(duration, loop){
    this->endAlpha = endAlpha;
    this->objectStartAlpha = true;
}

AlphaAction::AlphaAction(float startAlpha, float endAlpha, float duration, bool loop): TimeAction(duration, loop){
    this->startAlpha = startAlpha;
    this->endAlpha = endAlpha;
    this->objectStartAlpha = false;
}

AlphaAction::~AlphaAction(){

}

bool AlphaAction::run(){
    if (!TimeAction::run())
        return false;

    ConcreteObject* cObject = dynamic_cast<ConcreteObject*>(object);

    if (cObject && objectStartAlpha){
        startAlpha = cObject->getColor().w;
    }

    return true;
}

bool AlphaAction::step(){
    if (!TimeAction::step())
        return false;

    ConcreteObject* cObject = dynamic_cast<ConcreteObject*>(object);

    if (cObject){
        float alpha = (endAlpha - startAlpha) * value;
        Vector4 actualColor = cObject->getColor();
        cObject->setColor(actualColor.x, actualColor.y, actualColor.z, startAlpha + alpha);
    }

    return true;
}