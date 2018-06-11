
#include "GUIObject.h"

#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "Log.h"

#include <cmath>

using namespace Supernova;

GUIObject::GUIObject(): Mesh2D(){
    state = 0;
}

GUIObject::~GUIObject(){

}

int GUIObject::getState(){
    return state;
}

bool GUIObject::isCoordInside(float x, float y){
    Vector3 point = worldRotation.getRotationMatrix() * Vector3(x, y, 0);

    if (point.x >= (getWorldPosition().x - getCenter().x) and
        point.x <= (getWorldPosition().x - getCenter().x + abs(getWidth() * getWorldScale().x)) and
        point.y >= (getWorldPosition().y - getCenter().y) and
        point.y <= (getWorldPosition().y - getCenter().y + abs(getHeight() * getWorldScale().y))) {
        return true;
    }
    return false;
}

void GUIObject::engineOnDown(int pointer, float x, float y){
}

void GUIObject::engineOnUp(int pointer, float x, float y){
}

void GUIObject::engineOnTextInput(std::string text){
}
