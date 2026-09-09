// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT


#include "Input.h"

using namespace doriax;

std::map<int,bool> Input::keyPressed;
std::map<int,bool> Input::mousePressed;
Vector2 Input::mousePosition;
Vector2 Input::mouseScroll;
std::vector<Touch> Input::touches;
std::vector<Gamepad> Input::gamepads;
bool Input::mousedEntered;
int Input::modifiers;

void Input::addKeyPressed(int key){
    keyPressed[key] = true;
}

void Input::releaseKeyPressed(int key){
    keyPressed[key] = false;
}

void Input::addMousePressed(int button){
    mousePressed[button] = true;
}

void Input::releaseMousePressed(int button){
    mousePressed[button] = false;
}

void Input::setMousePosition(float x, float y){
    mousePosition.x = x;
    mousePosition.y = y;
}

void Input::setMouseScroll(float xoffset, float yoffset){
    mouseScroll.x = xoffset;
    mouseScroll.y = yoffset;
}

void Input::addTouch(int pointer, float x, float y){
    if (findTouchIndex(pointer) == -1)
        touches.push_back({pointer, Vector2(x, y)});
}

void Input::setTouchPosition(int pointer, float x, float y){
    size_t index = findTouchIndex(pointer);
    if (index >=0 && index < touches.size())
        touches[index].position = Vector2(x, y);
}

void Input::removeTouch(int pointer){
    size_t index = findTouchIndex(pointer);
    if (index != -1)
        touches.erase (touches.begin()+index);
}

void Input::clearTouches(){
    touches.clear();
}

void Input::addGamepad(int id, const std::string& name){
    size_t index = findGamepadIndex(id);
    if (index == -1){
        Gamepad gamepad;
        gamepad.id = id;
        gamepad.name = name;
        // Triggers rest at -1 in the [-1, 1] convention. Platforms seed their own
        // local state to -1 and skip the connect event, so seed the queryable
        // state here too or getGamepadAxis would report a resting trigger as 0.
        gamepad.axisValue[D_GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
        gamepad.axisValue[D_GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
        gamepads.push_back(gamepad);
    }else{
        gamepads[index].name = name;
    }
}

void Input::removeGamepad(int id){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        gamepads.erase(gamepads.begin()+index);
}

void Input::clearGamepads(){
    gamepads.clear();
}

void Input::addGamepadButtonPressed(int id, int button){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        gamepads[index].buttonPressed[button] = true;
}

void Input::releaseGamepadButtonPressed(int id, int button){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        gamepads[index].buttonPressed[button] = false;
}

void Input::setGamepadAxisValue(int id, int axis, float value){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        gamepads[index].axisValue[axis] = value;
}

void Input::addMouseEntered(){
    mousedEntered = true;
}

void Input::releaseMouseEntered(){
    mousedEntered = false;
}

void Input::setModifiers(int mods){
    modifiers = mods;
}

bool Input::isKeyPressed(int key){
    return keyPressed[key];
}

bool Input::isMousePressed(int button){
    return mousePressed[button];
}

bool Input::isTouch(){
    return (touches.size() > 0);
}

bool Input::isMouseEntered(){
    return mousedEntered;
}

Vector2 Input::getMousePosition(){
    return mousePosition;
}

Vector2 Input::getMouseScroll(){
    return mouseScroll;
}

Vector2 Input::getTouchPosition(int pointer){
    size_t index = findTouchIndex(pointer);
    if (index >=0 && index < touches.size())
        return touches[index].position;
    
    return Vector2(-1, -1); 
}

std::vector<Touch> Input::getTouches(){
    return touches;
}

size_t Input::numTouches(){
    return touches.size();
}

bool Input::isGamepadConnected(int id){
    return (findGamepadIndex(id) != -1);
}

std::string Input::getGamepadName(int id){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        return gamepads[index].name;

    return "";
}

bool Input::isGamepadButtonPressed(int id, int button){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        return gamepads[index].buttonPressed[button];

    return false;
}

float Input::getGamepadAxis(int id, int axis){
    size_t index = findGamepadIndex(id);
    if (index != -1)
        return gamepads[index].axisValue[axis];

    return 0.0f;
}

std::vector<Gamepad> Input::getGamepads(){
    return gamepads;
}

size_t Input::numGamepads(){
    return gamepads.size();
}

int Input::getGamepadId(size_t index){
    if (index < gamepads.size())
        return gamepads[index].id;

    return -1;
}

int Input::getModifiers(){
    return modifiers;
}

size_t Input::findTouchIndex(int pointer){
    size_t index = -1;
    for(int i = 0; i < touches.size(); i++){
        if (touches[i].pointer == pointer)
            index = i;
    }

    return index;
}

size_t Input::findGamepadIndex(int id){
    size_t index = -1;
    for(int i = 0; i < gamepads.size(); i++){
        if (gamepads[i].id == id)
            index = i;
    }

    return index;
}
