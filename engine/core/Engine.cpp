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
#include "ui/Button.h"
#include "LuaBind.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//#include "Mesh.h"

#include "audio/SoundManager.h"
#include "Input.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

//-----Supernova user config-----
Scene *Engine::mainScene;

int Engine::screenWidth;
int Engine::screenHeight;

int Engine::canvasWidth;
int Engine::canvasHeight;

int Engine::preferedCanvasWidth;
int Engine::preferedCanvasHeight;

Rect Engine::viewRect;

int Engine::renderAPI;
bool Engine::mouseAsTouch;
bool Engine::useDegrees;
Scaling Engine::scalingMode;
bool Engine::defaultNearestScaleTexture;
bool Engine::defaultResampleToPOTTexture;
bool Engine::fixedTimeObjectUpdate;
bool Engine::fixedTimePhysics;
bool Engine::fixedTimeAnimations;

unsigned long Engine::lastTime = 0;
float Engine::updateTimeCount = 0;

float Engine::deltatime = 0;
float Engine::framerate = 0;

float Engine::updateTime = 0.03;

//-----Supernova user events-----
FunctionCallback<void()> Engine::onCanvasLoaded;
FunctionCallback<void()> Engine::onCanvasChanged;
FunctionCallback<void()> Engine::onDraw;
FunctionCallback<void()> Engine::onUpdate;
FunctionCallback<void(int,float,float)> Engine::onTouchStart;
FunctionCallback<void(int,float,float)> Engine::onTouchEnd;
FunctionCallback<void(int,float,float)> Engine::onTouchDrag;
FunctionCallback<void(int,float,float)> Engine::onMouseDown;
FunctionCallback<void(int,float,float)> Engine::onMouseUp;
FunctionCallback<void(int,float,float)> Engine::onMouseDrag;
FunctionCallback<void(float,float)> Engine::onMouseMove;
FunctionCallback<void(int)> Engine::onKeyDown;
FunctionCallback<void(int)> Engine::onKeyUp;
FunctionCallback<void(std::string)> Engine::onTextInput;


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
    if (scalingMode == Scaling::FITWIDTH){
        Engine::canvasWidth = canvasWidth;
        Engine::canvasHeight = screenHeight * canvasWidth / screenWidth;
    }
    if (scalingMode == Scaling::FITHEIGHT){
        Engine::canvasHeight = canvasHeight;
        Engine::canvasWidth = screenWidth * canvasHeight / screenHeight;
    }
    
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

void Engine::setFixedTimeObjectUpdate(bool fixedTimeObjectUpdate) {
    Engine::fixedTimeObjectUpdate = fixedTimeObjectUpdate;
}

bool Engine::isFixedTimeObjectUpdate() {
    return fixedTimeObjectUpdate;
}

void Engine::setFixedTimePhysics(bool fixedTimePhysics){
    Engine::fixedTimePhysics = fixedTimePhysics;
}

bool Engine::isFixedTimePhysics(){
    return fixedTimePhysics;
}

void Engine::setFixedTimeAnimations(bool fixedTimeAnimations) {
    Engine::fixedTimeAnimations = fixedTimeAnimations;
}

bool Engine::isFixedTimeAnimations() {
    return fixedTimeAnimations;
}

void Engine::setUpdateTime(unsigned int updateTimeMS){
    Engine::updateTime = updateTimeMS / 1000.0f;
}

float Engine::getUpdateTime(){
    return Engine::updateTime;
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

    systemStart(0, 0);

}

void Engine::systemStart(int width, int height){

    Engine::setScreenSize(width, height);

    Engine::setMouseAsTouch(true);
    Engine::setUseDegrees(true);
    Engine::setRenderAPI(S_GLES2);
    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setDefaultNearestScaleTexture(false);
    Engine::setDefaultResampleToPOTTexture(true);
    Engine::setFixedTimeObjectUpdate(false);
    Engine::setFixedTimePhysics(false);
    Engine::setFixedTimeAnimations(false);
    
    auto now = std::chrono::steady_clock::now();
    lastTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    LuaBind::createLuaState();
    LuaBind::bind();
    
    init();
    
}

void Engine::systemSurfaceCreated(){

    if (Engine::getScene() != NULL){
        (Engine::getScene())->load();
    }

    onCanvasLoaded.call();
}

void Engine::systemSurfaceChanged(int width, int height) {

    Engine::setScreenSize(width, height);
    
    int viewX = 0;
    int viewY = 0;
    int viewWidth = Engine::getScreenWidth();
    int viewHeight = Engine::getScreenHeight();
    
    float screenAspect = (float)Engine::getScreenWidth() / (float)Engine::getScreenHeight();
    float canvasAspect = (float)Engine::getPreferedCanvasWidth() / (float)Engine::getPreferedCanvasHeight();
    
    //When canvas size is not changed
    if (Engine::getScalingMode() == Scaling::LETTERBOX){
        if (screenAspect < canvasAspect){
            float aspect = (float)Engine::getScreenWidth() / (float)Engine::getPreferedCanvasWidth();
            int newHeight = (int)((float)Engine::getPreferedCanvasHeight() * aspect);
            int dif = Engine::getScreenHeight() - newHeight;
            viewY = (dif/2);
            viewHeight = Engine::getScreenHeight()-(viewY*2); //diff could be odd, for this use view*2
        }else{
            float aspect = (float)Engine::getScreenHeight() / (float)Engine::getPreferedCanvasHeight();
            int newWidth = (int)((float)Engine::getPreferedCanvasWidth() * aspect);
            int dif = Engine::getScreenWidth() - newWidth;
            viewX = (dif/2);
            viewWidth = Engine::getScreenWidth()-(viewX*2);
        }
    }
    
    if (Engine::getScalingMode() == Scaling::CROP){
        if (screenAspect > canvasAspect){
            float aspect = (float)Engine::getScreenWidth() / (float)Engine::getPreferedCanvasWidth();
            int newHeight = (int)((float)Engine::getPreferedCanvasHeight() * aspect);
            int dif = Engine::getScreenHeight() - newHeight;
            viewY = (dif/2);
            viewHeight = Engine::getScreenHeight()-(viewY*2);
        }else{
            float aspect = (float)Engine::getScreenHeight() / (float)Engine::getPreferedCanvasHeight();
            int newWidth = (int)((float)Engine::getPreferedCanvasWidth() * aspect);
            int dif = Engine::getScreenWidth() - newWidth;
            viewX = (dif/2);
            viewWidth = Engine::getScreenWidth()-(viewX*2);
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

        if (Engine::getScene())
            (Engine::getScene())->update();

        Engine::onUpdate.call();
    }
    if (updateLoops > 100){
        Log::Warn("More than 100 updates in a frame");
    }

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
    x = (x * (float)screenWidth / viewRect.getWidth());
    y = (y * (float)screenHeight / viewRect.getHeight());
    
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

        if (mainScene) {
            std::vector<UIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engineOnDown(pointer, x, y);
            }
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

        if (mainScene) {
            std::vector<UIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engineOnUp(pointer, x, y);
            }
        }
    }
}

void Engine::systemTouchDrag(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onTouchDrag.call(pointer, x, y);
        Input::setTouchPosition(x, y);
        //-----------------
    }
}

void Engine::systemMouseDown(int button, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseDown.call(button, x, y);
        Input::addMousePressed(button);
        Input::setMousePosition(x, y);
        //-----------------
        if (Engine::isMouseAsTouch()){
            //-----------------
            Engine::onTouchStart.call(100, x, y);
            Input::addTouchStarted();
            Input::setTouchPosition(x, y);
            //-----------------
        }

        if (mainScene) {
            std::vector<UIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engineOnDown(button, x, y);
            }
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
        if (Engine::isMouseAsTouch()){
            //-----------------
            Engine::onTouchEnd.call(100, x, y);
            Input::releaseTouchStarted();
            Input::setTouchPosition(x, y);
            //-----------------
        }

        if (mainScene) {
            std::vector<UIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engineOnUp(button, x, y);
            }
        }
    }
}

void Engine::systemMouseDrag(int button, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseDrag.call(button, x, y);
        Input::setMousePosition(x, y);
        //-----------------
        if (Engine::isMouseAsTouch()){
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

    if (mainScene) {
        std::vector<UIObject*>::iterator it;
        for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
            (*it)->engineOnTextInput(text);
        }
    }
}
