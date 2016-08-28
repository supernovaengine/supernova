#include "Engine.h"

#include <iostream>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Supernova.h"
#include "platform/Log.h"

#include "LuaBinding.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Mesh.h"
#include "Input.h"

/*
#include "Shape.h"
void teste(){
    printf("oiii \n");
}
*/


void Engine::onStart(){

    onStart(0, 0);

}

void Engine::onStart(int width, int height){

    Supernova::setScreenSize(width, height);

    Supernova::setMouseAsTouch(true);
    Supernova::setUseDegrees(true);
    Supernova::setRenderAPI(S_GLES2);

    Supernova::setLuaState(luaL_newstate());

    LuaBinding::bind();

    init();
}

void Engine::onSurfaceCreated(){

    if (Supernova::getScene() != NULL){
        (Supernova::getScene())->load();
    }

}

void Engine::onSurfaceChanged(int width, int height) {

    Supernova::setScreenSize(width, height);

    if (Supernova::getScene() != NULL){
        (Supernova::getScene())->screenSize(Supernova::getScreenWidth(), Supernova::getScreenHeight());
    }

}

void Engine::onDrawFrame() {

	Supernova::call_onFrame();

    if (Supernova::getScene() != NULL){
        (Supernova::getScene())->draw();
    }
}

void Engine::onTouchPress(float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onTouchPress(x, y);
}

void Engine::onTouchUp(float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onTouchUp(x, y);
}

void Engine::onTouchDrag(float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onTouchDrag(x, y);
}

void Engine::onMousePress(int button, float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onMousePress(button, x, y);
    if ((Supernova::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
        Supernova::call_onTouchPress(x, y);
    }
}
void Engine::onMouseUp(int button, float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onMouseUp(button, x, y);
    if ((Supernova::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
        Supernova::call_onTouchUp(x, y);
    }
}

void Engine::onMouseDrag(int button, float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onMouseDrag(button, x, y);
    if ((Supernova::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
        Supernova::call_onTouchDrag(x, y);
    }
}

void Engine::onMouseMove(float x, float y){
    x = (Supernova::getCanvasWidth() * (x+1)) / 2;
    y = (Supernova::getCanvasHeight() * (y+1)) / 2;

    Supernova::call_onMouseMove(x, y);
}

void Engine::onKeyPress(int inputKey){
    Supernova::call_onKeyPress(inputKey);
    //printf("keypress %i\n", inputKey);
}

void Engine::onKeyUp(int inputKey){
    Supernova::call_onKeyUp(inputKey);
    //printf("keyup %i\n", inputKey);
}

void Engine::onTextInput(const char* text){
    //printf("textinput %s\n", text);
    Log::Verbose(LOG_TAG,"textinput %s\n", text);
}

int Engine::getScreenWidth(){
    return Supernova::getScreenWidth();
}

int Engine::getScreenHeight(){
    return Supernova::getScreenHeight();
}
