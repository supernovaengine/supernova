#include "UIObject.h"

//
// (c) 2018 Eduardo Doria.
//

#include "Log.h"

#include <cmath>

using namespace Supernova;

UIObject::UIObject(): Mesh2D(){
    state = 0;
    pointerDown = -1;
    focused = false;
}

UIObject::~UIObject(){

}

int UIObject::getState(){
    return state;
}

bool UIObject::isCoordInside(float x, float y){
    Vector3 point = worldRotation.getRotationMatrix() * Vector3(x, y, 0);

    if (point.x >= (getWorldPosition().x - getCenter().x) and
        point.x <= (getWorldPosition().x - getCenter().x + abs(getWidth() * getWorldScale().x)) and
        point.y >= (getWorldPosition().y - getCenter().y) and
        point.y <= (getWorldPosition().y - getCenter().y + abs(getHeight() * getWorldScale().y))) {
        return true;
    }
    return false;
}

void UIObject::inputDown(){

}

void UIObject::inputUp(){

}

void UIObject::engineOnDown(int pointer, float x, float y){
    if (isCoordInside(x, y)) {

        pointerDown = pointer;
        focused = true;

        inputDown();
    }
}

void UIObject::engineOnUp(int pointer, float x, float y){
    if (pointerDown == pointer) {

        pointerDown = -1;
        focused = false;

        inputUp();
    }
}

void UIObject::engineOnTextInput(std::string text){
}
