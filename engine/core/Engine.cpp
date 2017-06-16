#include "Engine.h"

#include <iostream>

#include "Supernova.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "Scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Events.h"
#include "platform/Log.h"

#include "LuaBind.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Mesh.h"
#include "Input.h"

#include "audio/SoundManager.h"

using namespace Supernova;

Scene *Engine::mainScene;

int Engine::screenWidth;
int Engine::screenHeight;

int Engine::canvasWidth;
int Engine::canvasHeight;

int Engine::preferedCanvasWidth;
int Engine::preferedCanvasHeight;

int Engine::renderAPI;
bool Engine::mouseAsTouch;
bool Engine::useDegrees;
int Engine::scalingMode;


Engine::Engine() {
}

Engine::~Engine() {
    
}

void Engine::setScene(Scene *mainScene){
    Engine::mainScene = mainScene;
}

Scene* Engine::getScene(){
    return mainScene;
}

int Engine::getScreenWidth(){
    return Engine::screenWidth;
}

int Engine::getScreenHeight(){
    return Engine::screenHeight;
}

void Engine::setScreenSize(int screenWidth, int screenHeight){
    
    Engine::screenWidth = screenWidth;
    Engine::screenHeight = screenHeight;
    
    if ((Engine::preferedCanvasWidth != 0) && (Engine::preferedCanvasHeight != 0)){
        setCanvasSize(preferedCanvasWidth, preferedCanvasHeight);
    }
    
}

int Engine::getCanvasWidth(){
    return Engine::canvasWidth;
}

int Engine::getCanvasHeight(){
    return Engine::canvasHeight;
}

void Engine::setCanvasSize(int canvasWidth, int canvasHeight){
    
    Engine::canvasWidth = canvasWidth;
    Engine::canvasHeight = canvasHeight;
    
    if ((Engine::screenWidth == 0) || (Engine::screenHeight == 0)){
        setScreenSize(canvasWidth, canvasHeight);
    }
    
    //When canvas size is changed
    if (scalingMode == S_SCALING_FITWIDTH){
        Engine::canvasWidth = canvasWidth;
        Engine::canvasHeight = screenHeight * canvasWidth / screenWidth;
    }
    if (scalingMode == S_SCALING_FITHEIGHT){
        Engine::canvasHeight = canvasHeight;
        Engine::canvasWidth = screenWidth * canvasHeight / screenHeight;
    }
    // S_SCALING_STRETCH do not need nothing
    
    if ((Engine::preferedCanvasWidth == 0) && (Engine::preferedCanvasHeight == 0)){
        setPreferedCanvasSize(canvasWidth, canvasHeight);
    }
    
}

int Engine::getPreferedCanvasWidth(){
    return Engine::preferedCanvasWidth;
}

int Engine::getPreferedCanvasHeight(){
    return Engine::preferedCanvasHeight;
}

void Engine::setPreferedCanvasSize(int preferedCanvasWidth, int preferedCanvasHeight){
    if ((Engine::preferedCanvasWidth == 0) && (Engine::preferedCanvasHeight == 0)){
        Engine::preferedCanvasWidth = preferedCanvasWidth;
        Engine::preferedCanvasHeight = preferedCanvasHeight;
    }
}

void Engine::setRenderAPI(int renderAPI){
    Engine::renderAPI = renderAPI;
}

int Engine::getRenderAPI(){
    return renderAPI;
}

void Engine::setScalingMode(int scalingMode){
    Engine::scalingMode = scalingMode;
}

int Engine::getScalingMode(){
    return scalingMode;
}

void Engine::setMouseAsTouch(bool mouseAsTouch){
    Engine::mouseAsTouch = mouseAsTouch;
}

bool Engine::isMouseAsTouch(){
    return Engine::mouseAsTouch;
}

void Engine::setUseDegrees(bool useDegrees){
    Engine::useDegrees = useDegrees;
}

bool Engine::isUseDegrees(){
    return Engine::useDegrees;
}

int Engine::getPlatform(){
    
#ifdef SUPERNOVA_IOS
    return S_IOS;
#endif
    
#ifdef SUPERNOVA_ANDROID
    return S_ANDROID;
#endif
    
#ifdef SUPERNOVA_WEB
    return S_WEB;
#endif
    
    return 0;
}


void Engine::onStart(){

    onStart(0, 0);

}

void Engine::onStart(int width, int height){

    Engine::setScreenSize(width, height);

    Engine::setMouseAsTouch(true);
    Engine::setUseDegrees(true);
    Engine::setRenderAPI(S_GLES2);
    Engine::setScalingMode(S_SCALING_FITWIDTH);

    LuaBind::createLuaState();
    LuaBind::bind();

    init();
}

void Engine::onSurfaceCreated(){

    if (Engine::getScene() != NULL){
        (Engine::getScene())->load();
    }

}

void Engine::onSurfaceChanged(int width, int height) {

    Engine::setScreenSize(width, height);

    if (Engine::getScene() != NULL){
        (Engine::getScene())->updateViewSize();
    }

}

void Engine::onDrawFrame() {

	Events::call_onFrame();

    if (Engine::getScene() != NULL){
        (Engine::getScene())->update();
    }

    SoundManager::checkActive();
}

void Engine::onPause(){
    SoundManager::pauseAll();
}

void Engine::onResume(){
    SoundManager::resumeAll();
}

void Engine::onTouchPress(float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onTouchPress(x, y);
}

void Engine::onTouchUp(float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onTouchUp(x, y);
}

void Engine::onTouchDrag(float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onTouchDrag(x, y);
}

void Engine::onMousePress(int button, float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onMousePress(button, x, y);
    if ((Engine::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
        Events::call_onTouchPress(x, y);
    }
}
void Engine::onMouseUp(int button, float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onMouseUp(button, x, y);
    if ((Engine::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
        Events::call_onTouchUp(x, y);
    }
}

void Engine::onMouseDrag(int button, float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onMouseDrag(button, x, y);
    if ((Engine::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
        Events::call_onTouchDrag(x, y);
    }
}

void Engine::onMouseMove(float x, float y){
    x = (Engine::getCanvasWidth() * (x+1)) / 2;
    y = (Engine::getCanvasHeight() * (y+1)) / 2;

    Events::call_onMouseMove(x, y);
}

void Engine::onKeyPress(int inputKey){
    Events::call_onKeyPress(inputKey);
    //printf("keypress %i\n", inputKey);
}

void Engine::onKeyUp(int inputKey){
    Events::call_onKeyUp(inputKey);
    //printf("keyup %i\n", inputKey);
}

void Engine::onTextInput(const char* text){
    //printf("textinput %s\n", text);
    Log::Verbose(LOG_TAG,"textinput %s\n", text);
}
