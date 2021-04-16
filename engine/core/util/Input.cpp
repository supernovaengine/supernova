#include "Input.h"

using namespace Supernova;

std::map<int,bool> Input::keyPressed;
std::map<int,bool> Input::mousePressed;
bool Input::touchStarted;

Vector2 Input::mousePosition;
Vector2 Input::touchPosition;


void Input::addKeyPressed(int key){
    keyPressed[key] = true;
}

void Input::releaseKeyPressed(int key){
    keyPressed[key] = false;
}

void Input::addMousePressed(int key){
    mousePressed[key] = true;
}

void Input::releaseMousePressed(int key){
    mousePressed[key] = false;
}

void Input::setMousePosition(float x, float y){
    mousePosition.x = x;
    mousePosition.y = y;
}

void Input::addTouchStarted(){
    touchStarted = true;
}

void Input::releaseTouchStarted(){
    touchStarted = false;
}

void Input::setTouchPosition(float x, float y){
    touchPosition.x = x;
    touchPosition.y = y;
}

bool Input::isKeyPressed(int key){
    if (keyPressed[key])
        return true;
    
    return false;
}

bool Input::isMousePressed(int key){
    if (mousePressed[key])
        return true;
    
    return false;
}

bool Input::isTouch(){
    if (touchStarted)
        return true;
    
    return false;
}

Vector2 Input::getMousePosition(){
    return mousePosition;
}

Vector2 Input::getTouchPosition(){
    return touchPosition;
}
