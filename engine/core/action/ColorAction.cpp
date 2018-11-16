#include "ColorAction.h"

#include "Object.h"
#include "GraphicObject.h"


using namespace Supernova;

ColorAction::ColorAction(Vector4 endColor, float duration, bool loop): TimeAction(duration, loop){
    this->endColor = endColor;
    this->objectStartColor = true;
    this->useAlpha = true;
}

ColorAction::ColorAction(Vector4 startColor, Vector4 endColor, float duration, bool loop): TimeAction(duration, loop){
    this->startColor = startColor;
    this->endColor = endColor;
    this->objectStartColor = false;
    this->useAlpha = true;
}

ColorAction::ColorAction(float endRed, float endGreen, float endBlue, float duration, bool loop): TimeAction(duration, loop){
    this->endColor = Vector4(endRed, endGreen, endBlue, 0.0);
    this->objectStartColor = true;
    this->useAlpha = false;
}

ColorAction::ColorAction(float startRed, float startGreen, float startBlue, float endRed, float endGreen, float endBlue, float duration, bool loop): TimeAction(duration, loop){
    this->startColor = Vector4(startRed, startGreen, startBlue, 0.0);
    this->endColor = Vector4(endRed, endGreen, endBlue, 0.0);
    this->objectStartColor = false;
    this->useAlpha = false;
}

ColorAction::~ColorAction(){

}

bool ColorAction::run(){
    if (!TimeAction::run())
        return false;

    GraphicObject* cObject = dynamic_cast<GraphicObject*>(object);

    if (cObject && objectStartColor){
        startColor = cObject->getColor();
    }

    return true;
}

bool ColorAction::step(){
    if (!TimeAction::step())
        return false;

    GraphicObject* cObject = dynamic_cast<GraphicObject*>(object);

    if (cObject){
        Vector4 color = (endColor - startColor) * value;
        if (useAlpha) {
            cObject->setColor(startColor + color);
        }else{
            float actualAlpha = cObject->getColor().w;
            cObject->setColor(Vector4(startColor.x, startColor.y, startColor.z, actualAlpha) + color);
        }
    }

    return true;
}
