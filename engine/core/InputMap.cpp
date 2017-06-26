#include "InputMap.h"

#include "Input.h"

using namespace Supernova;

std::map<int,bool> InputMap::keyPressed;
std::map<int,bool> InputMap::mousePressed;
bool InputMap::touchPressed;

Vector2 InputMap::mousePosition;
Vector2 InputMap::touchPosition;


void InputMap::addKeyPressed(int key){
    keyPressed[key] = true;
}

void InputMap::releaseKeyPressed(int key){
    keyPressed[key] = false;
}

void InputMap::addMousePressed(int key){
    mousePressed[key] = true;
}

void InputMap::releaseMousePressed(int key){
    mousePressed[key] = false;
}

void InputMap::setMousePosition(float x, float y){
    mousePosition.x = x;
    mousePosition.y = y;
}

void InputMap::addTouchPressed(){
    touchPressed = true;
}

void InputMap::releaseTouchPressed(){
    touchPressed = false;
}

void InputMap::setTouchPosition(float x, float y){
    touchPosition.x = x;
    touchPosition.y = y;
}

bool InputMap::isKeyPressed(int key){
    if (keyPressed[key])
        return true;
    
    return false;
}

bool InputMap::isMousePressed(int key){
    if (mousePressed[key])
        return true;
    
    return false;
}

bool InputMap::isTouch(){
    if (touchPressed)
        return true;
    
    return false;
}

Vector2 InputMap::getMousePosition(){
    return mousePosition;
}

Vector2 InputMap::getTouchPosition(){
    return touchPosition;
}
