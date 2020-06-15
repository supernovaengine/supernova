#include "Engine.h"

#include <iostream>

#include "Supernova.h"

#include "Scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "math/Rect.h"
#include "Log.h"
#include "script/LuaBinding.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//#include "Mesh.h"

#include "audio/SoundManager.h"
#include "system/System.h"
#include "Input.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

//-----Supernova user config-----
Scene *Engine::mainScene;

int Engine::canvasWidth;
int Engine::canvasHeight;

int Engine::preferedCanvasWidth;
int Engine::preferedCanvasHeight;

Rect Engine::viewRect;

int Engine::renderAPI;
bool Engine::callMouseInTouchEvent;
bool Engine::callTouchInMouseEvent;
bool Engine::useDegrees;
Scaling Engine::scalingMode;
bool Engine::defaultNearestScaleTexture;
bool Engine::defaultResampleToPOTTexture;
bool Engine::fixedTimeSceneUpdate;
bool Engine::fixedTimePhysics;
bool Engine::fixedTimeAnimations;

unsigned long Engine::lastTime = 0;
float Engine::updateTimeCount = 0;

float Engine::deltatime = 0;
float Engine::framerate = 0;

float Engine::updateTime = 0.03;

//-----Supernova user events-----
FunctionSubscribe<void()> Engine::onCanvasLoaded;
FunctionSubscribe<void()> Engine::onCanvasChanged;
FunctionSubscribe<void()> Engine::onDraw;
FunctionSubscribe<void()> Engine::onUpdate;
FunctionSubscribe<void(int,float,float)> Engine::onTouchStart;
FunctionSubscribe<void(int,float,float)> Engine::onTouchEnd;
FunctionSubscribe<void(int,float,float)> Engine::onTouchDrag;
FunctionSubscribe<void(int,float,float)> Engine::onMouseDown;
FunctionSubscribe<void(int,float,float)> Engine::onMouseUp;
FunctionSubscribe<void(int,float,float)> Engine::onMouseDrag;
FunctionSubscribe<void(float,float)> Engine::onMouseMove;
FunctionSubscribe<void(int)> Engine::onKeyDown;
FunctionSubscribe<void(int)> Engine::onKeyUp;
FunctionSubscribe<void(std::string)> Engine::onTextInput;


Engine::Engine() {
    this->mainScene = NULL;
}

Engine::~Engine() {
    
}

void Engine::setScene(Scene *mainScene){
    Engine::mainScene = mainScene;
}

Scene* Engine::getScene(){
    return mainScene;
}

int Engine::getCanvasWidth(){
    return Engine::canvasWidth;
}

int Engine::getCanvasHeight(){
    return Engine::canvasHeight;
}

void Engine::setCanvasSize(int canvasWidth, int canvasHeight){
    Engine::preferedCanvasWidth = canvasWidth;
    Engine::preferedCanvasHeight = canvasHeight;

    calculateCanvas();
}

void Engine::calculateCanvas(){
    Engine::canvasWidth = preferedCanvasWidth;
    Engine::canvasHeight = preferedCanvasHeight;

    int screenWidth = System::instance().getScreenWidth();
    int screenHeight = System::instance().getScreenHeight();

    //When canvas size is changed
    if (scalingMode == Scaling::FITWIDTH){
        Engine::canvasWidth = preferedCanvasWidth;
        Engine::canvasHeight = screenHeight * preferedCanvasWidth / screenWidth;
    }
    if (scalingMode == Scaling::FITHEIGHT){
        Engine::canvasHeight = preferedCanvasHeight;
        Engine::canvasWidth = screenWidth * preferedCanvasHeight / screenHeight;
    }
}

int Engine::getPreferedCanvasWidth(){
    return Engine::preferedCanvasWidth;
}

int Engine::getPreferedCanvasHeight(){
    return Engine::preferedCanvasHeight;
}

Rect* Engine::getViewRect(){
    return &viewRect;
}

void Engine::setRenderAPI(int renderAPI){
    Engine::renderAPI = renderAPI;
}

int Engine::getRenderAPI(){
    return renderAPI;
}

void Engine::setScalingMode(Scaling scalingMode){
    Engine::scalingMode = scalingMode;
}

int Engine::getScalingMode(){
    return scalingMode;
}

void Engine::setCallMouseInTouchEvent(bool callMouseInTouchEvent){
    Engine::callMouseInTouchEvent = callMouseInTouchEvent;
}
bool Engine::isCallMouseInTouchEvent(){
    return Engine::callMouseInTouchEvent;
}

void Engine::setCallTouchInMouseEvent(bool callTouchInMouseEvent){
    Engine::callTouchInMouseEvent = callTouchInMouseEvent;
}
bool Engine::isCallTouchInMouseEvent(){
    return Engine::callTouchInMouseEvent;
}

void Engine::setUseDegrees(bool useDegrees){
    Engine::useDegrees = useDegrees;
}

bool Engine::isUseDegrees(){
    return Engine::useDegrees;
}

void Engine::setDefaultNearestScaleTexture(bool defaultNearestScaleTexture){
    Engine::defaultNearestScaleTexture = defaultNearestScaleTexture;
}

bool Engine::isDefaultNearestScaleTexture(){
    return defaultNearestScaleTexture;
}

void Engine::setDefaultResampleToPOTTexture(bool defaultResampleToPOTTexture){
    Engine::defaultResampleToPOTTexture = defaultResampleToPOTTexture;
}

bool Engine::isDefaultResampleToPOTTexture(){
    return defaultResampleToPOTTexture;
}

void Engine::setFixedTimeSceneUpdate(bool fixedTimeSceneUpdate) {
    Engine::fixedTimeSceneUpdate = fixedTimeSceneUpdate;
}

bool Engine::isFixedTimeSceneUpdate() {
    return fixedTimeSceneUpdate;
}

void Engine::setUpdateTime(unsigned int updateTimeMS){
    Engine::updateTime = updateTimeMS / 1000.0f;
}

float Engine::getUpdateTime(){
    return Engine::updateTime;
}

float Engine::getSceneUpdateTime(){
    if (isFixedTimeSceneUpdate()){
        return getUpdateTime();
    }else{
        return getDeltatime();
    }
}


int Engine::getPlatform(){
    
#ifdef SUPERNOVA_IOS
    return S_PLATFORM_IOS;
#endif
    
#ifdef SUPERNOVA_ANDROID
    return S_PLATFORM_ANDROID;
#endif
    
#ifdef SUPERNOVA_WEB
    return S_PLATFORM_WEB;
#endif
    
    return 0;
}

float Engine::getFramerate(){
    return framerate;
}

float Engine::getDeltatime(){
    return deltatime;
}

void Engine::systemStart(){

    Engine::setCanvasSize(1000,480);
    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setCallMouseInTouchEvent(false);
    Engine::setCallTouchInMouseEvent(false);
    Engine::setUseDegrees(true);
    Engine::setRenderAPI(S_GLES2);
    Engine::setDefaultNearestScaleTexture(false);
    Engine::setDefaultResampleToPOTTexture(true);
    Engine::setFixedTimeSceneUpdate(false);
    
    auto now = std::chrono::steady_clock::now();
    lastTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    LuaBinding::createLuaState();
    LuaBinding::bind();
    
    init();
    
}

void Engine::systemSurfaceCreated(){

    if (Engine::getScene() != NULL){
        (Engine::getScene())->load();
    }

    onCanvasLoaded.call();
}

void Engine::systemSurfaceChanged() {

    calculateCanvas();

    int screenWidth = System::instance().getScreenWidth();
    int screenHeight = System::instance().getScreenHeight();
    
    int viewX = 0;
    int viewY = 0;
    int viewWidth = screenWidth;
    int viewHeight = screenHeight;
    
    float screenAspect = (float)screenWidth / (float)screenHeight;
    float canvasAspect = (float)Engine::getPreferedCanvasWidth() / (float)Engine::getPreferedCanvasHeight();
    
    //When canvas size is not changed
    if (Engine::getScalingMode() == Scaling::LETTERBOX){
        if (screenAspect < canvasAspect){
            float aspect = (float)screenWidth / (float)Engine::getPreferedCanvasWidth();
            int newHeight = (int)((float)Engine::getPreferedCanvasHeight() * aspect);
            int dif = screenHeight - newHeight;
            viewY = (dif/2);
            viewHeight = screenHeight-(viewY*2); //diff could be odd, for this use view*2
        }else{
            float aspect = (float)screenHeight / (float)Engine::getPreferedCanvasHeight();
            int newWidth = (int)((float)Engine::getPreferedCanvasWidth() * aspect);
            int dif = screenWidth - newWidth;
            viewX = (dif/2);
            viewWidth = screenWidth-(viewX*2);
        }
    }
    
    if (Engine::getScalingMode() == Scaling::CROP){
        if (screenAspect > canvasAspect){
            float aspect = (float)screenWidth / (float)Engine::getPreferedCanvasWidth();
            int newHeight = (int)((float)Engine::getPreferedCanvasHeight() * aspect);
            int dif = screenHeight - newHeight;
            viewY = (dif/2);
            viewHeight = screenHeight-(viewY*2);
        }else{
            float aspect = (float)screenHeight / (float)Engine::getPreferedCanvasHeight();
            int newWidth = (int)((float)Engine::getPreferedCanvasWidth() * aspect);
            int dif = screenWidth - newWidth;
            viewX = (dif/2);
            viewWidth = screenWidth-(viewX*2);
        }
    }
    
    // S_SCALING_STRETCH do not need nothing
    
    viewRect.setRect(viewX, viewY, viewWidth, viewHeight);

    if (Engine::getScene() != NULL){
        (Engine::getScene())->updateCameraSize();
    }

    onCanvasChanged.call();
}

void Engine::systemDraw() {
    
    auto now = std::chrono::steady_clock::now();
    unsigned long newTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    deltatime = (newTime - lastTime) / 1000.0f;
    lastTime = newTime;
    framerate = 1 / (float)deltatime;
    
    int updateLoops = 0;
    updateTimeCount += deltatime;
    while (updateTimeCount >= updateTime && updateLoops <= 100){
        updateLoops++;
        updateTimeCount -= updateTime;

        Engine::onUpdate.call();

        if (isFixedTimeSceneUpdate() && Engine::getScene())
            (Engine::getScene())->update();
    }
    if (updateLoops > 100){
        Log::Warn("More than 100 updates in a frame");
    }

    if (!isFixedTimeSceneUpdate() && Engine::getScene())
        (Engine::getScene())->update();

    Engine::onDraw.call();

    if (Engine::getScene())
        (Engine::getScene())->draw();
    
    SoundManager::checkActive();
    
}

void Engine::systemPause(){
    SoundManager::pauseAll();
}

void Engine::systemResume(){
    SoundManager::resumeAll();
}

bool Engine::transformCoordPos(float& x, float& y){
    x = (x * (float)System::instance().getScreenWidth() / viewRect.getWidth());
    y = (y * (float)System::instance().getScreenHeight() / viewRect.getHeight());
    
    x = ((float)Engine::getCanvasWidth() * (x+1)) / 2;
    y = ((float)Engine::getCanvasHeight() * (y+1)) / 2;
    
    return ((x >= 0) && (x <= Engine::getCanvasWidth()) && (y >= 0) && (y <= Engine::getCanvasHeight()));
}

void Engine::systemTouchStart(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onTouchStart.call(pointer, x, y);
        Input::addTouchStarted();
        Input::setTouchPosition(x, y);
        //-----------------
        if (Engine::isCallMouseInTouchEvent()){
            //-----------------
            Engine::onMouseDown.call(100, x, y);
            Input::addMousePressed(100);
            Input::setMousePosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemTouchEnd(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onTouchEnd.call(pointer, x, y);
        Input::releaseTouchStarted();
        Input::setTouchPosition(x, y);
        //-----------------
        if (Engine::isCallMouseInTouchEvent()){
            //-----------------
            Engine::onMouseUp.call(100, x, y);
            Input::releaseMousePressed(100);
            Input::setMousePosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemTouchDrag(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onTouchDrag.call(pointer, x, y);
        Input::setTouchPosition(x, y);
        //-----------------
        if (Engine::isCallMouseInTouchEvent()){
            //-----------------
            Engine::onMouseDrag.call(100, x, y);
            Input::setMousePosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemMouseDown(int button, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseDown.call(button, x, y);
        Input::addMousePressed(button);
        Input::setMousePosition(x, y);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchStart.call(100, x, y);
            Input::addTouchStarted();
            Input::setTouchPosition(x, y);
            //-----------------
        }
    }
}
void Engine::systemMouseUp(int button, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseUp.call(button, x, y);
        Input::releaseMousePressed(button);
        Input::setMousePosition(x, y);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchEnd.call(100, x, y);
            Input::releaseTouchStarted();
            Input::setTouchPosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemMouseDrag(int button, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseDrag.call(button, x, y);
        Input::setMousePosition(x, y);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchDrag.call(100, x, y);
            Input::setTouchPosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemMouseMove(float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseMove.call(x, y);
        Input::setMousePosition(x, y);
        //-----------------
    }
}

void Engine::systemKeyDown(int inputKey){
    //-----------------
    Engine::onKeyDown.call(inputKey);
    Input::addKeyPressed(inputKey);
    //-----------------
}

void Engine::systemKeyUp(int inputKey){
    //-----------------
    Engine::onKeyUp.call(inputKey);
    Input::releaseKeyPressed(inputKey);
    //-----------------
}

void Engine::systemTextInput(const char* text){
    onTextInput.call(text);
}
