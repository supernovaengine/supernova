#include "UIObject.h"

//
// (c) 2018 Eduardo Doria.
//

#include "Log.h"

#include <cmath>
#include "Engine.h"
#include "util/UniqueToken.h"

using namespace Supernova;

UIObject::UIObject(): Mesh2D(){
    state = 0;
    pointerDown = -1;
    focused = false;

    eventId = UniqueToken::get();

    Engine::onTextInput.add<UIObject, &UIObject::engineOnTextInput>(eventId, this);
    Engine::onTouchStart.add<UIObject, &UIObject::engineOnDown>(eventId, this);
    Engine::onMouseDown.add<UIObject, &UIObject::engineOnDown>(eventId, this);
    Engine::onTouchEnd.add<UIObject, &UIObject::engineOnUp>(eventId, this);
    Engine::onMouseUp.add<UIObject, &UIObject::engineOnUp>(eventId, this);
}

UIObject::~UIObject(){
    Engine::onTextInput.remove(eventId);
    Engine::onTouchStart.remove(eventId);
    Engine::onMouseDown.remove(eventId);
    Engine::onTouchEnd.remove(eventId);
    Engine::onMouseUp.remove(eventId);
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

void UIObject::getFocus(){

}

void UIObject::lostFocus(){

}

void UIObject::engineOnDown(int pointer, float x, float y){
    if (isCoordInside(x, y)) {
        pointerDown = pointer;
        focused = true;
        inputDown();
        getFocus();
    }else{
        focused = false;
        lostFocus();
    }
}

void UIObject::engineOnUp(int pointer, float x, float y){
    if (pointerDown == pointer) {
        pointerDown = -1;
        inputUp();
    }
}

void UIObject::engineOnTextInput(std::string text){
}
