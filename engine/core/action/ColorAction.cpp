#include "ColorAction.h"

#include "Object.h"
#include "ConcreteObject.h"


using namespace Supernova;

ColorAction::ColorAction(Vector4 endColor, float duration, bool loop): TimeAction(duration, loop){
    this->endColor = endColor;
    this->objectStartColor = true;
}

ColorAction::ColorAction(Vector4 startColor, Vector4 endColor, float duration, bool loop): TimeAction(duration, loop){
    this->startColor = startColor;
    this->endColor = endColor;
    this->objectStartColor = false;
}

ColorAction::ColorAction(float endRed, float endGreen, float endBlue, float duration, bool loop)
        :ColorAction(Vector4(endRed, endGreen, endBlue, 1.0), duration, loop){
}

ColorAction::ColorAction(float startRed, float startGreen, float startBlue, float endRed, float endGreen, float endBlue, float duration, bool loop)
        :ColorAction(Vector4(startRed, startGreen, startBlue, 1.0), Vector4(endRed, endGreen, endBlue, 1.0), duration, loop){
}

ColorAction::~ColorAction(){

}

bool ColorAction::run(){
    if (!TimeAction::run())
        return false;

    ConcreteObject* cObject = dynamic_cast<ConcreteObject*>(object);

    if (cObject && objectStartColor){
        startColor = cObject->getColor();
    }

    return true;
}

bool ColorAction::step(){
    if (!TimeAction::step())
        return false;

    ConcreteObject* cObject = dynamic_cast<ConcreteObject*>(object);

    if (cObject){
        Vector4 color = (endColor - startColor) * value;
        cObject->setColor(startColor + color);
    }

    return true;
}
