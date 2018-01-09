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
#include <time.h>

#include "Events.h"
#include "math/Rect.h"
#include "platform/Log.h"

#include "LuaBind.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Mesh.h"
#include "InputCode.h"

#include "audio/SoundManager.h"


using namespace Supernova;

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
int Engine::scalingMode;
bool Engine::nearestScaleTexture;

unsigned long Engine::lastTime = 0;
unsigned int Engine::updateTimeCount = 0;

unsigned int Engine::deltatime = 0;
float Engine::framerate = 0;

unsigned int Engine::updateTime = 30;


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
    if (scalingMode == S_SCALING_FITWIDTH){
        Engine::canvasWidth = canvasWidth;
        Engine::canvasHeight = screenHeight * canvasWidth / screenWidth;
    }
    if (scalingMode == S_SCALING_FITHEIGHT){
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

void Engine::setNearestScaleTexture(bool nearestScaleTexture){
    Engine::nearestScaleTexture = nearestScaleTexture;
}

bool Engine::isNearestScaleTexture(){
    return nearestScaleTexture;
}

void Engine::setUpdateTime(unsigned int updateTime){
    Engine::updateTime = updateTime;
}

unsigned int Engine::getUpdateTime(){
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

void Engine::onStart(){

    onStart(0, 0);

}

void Engine::onStart(int width, int height){

    Engine::setScreenSize(width, height);

    Engine::setMouseAsTouch(true);
    Engine::setUseDegrees(true);
    Engine::setRenderAPI(S_GLES2);
    Engine::setScalingMode(S_SCALING_FITWIDTH);
    Engine::setNearestScaleTexture(false);

    LuaBind::createLuaState();
    LuaBind::bind();
    
    auto now = std::chrono::steady_clock::now();
    lastTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    init();
}

void Engine::onSurfaceCreated(){

    if (Engine::getScene() != NULL){
        (Engine::getScene())->load();
    }

}

void Engine::onSurfaceChanged(int width, int height) {

    Engine::setScreenSize(width, height);
    
    int viewX = 0;
    int viewY = 0;
    int viewWidth = Engine::getScreenWidth();
    int viewHeight = Engine::getScreenHeight();
    
    float screenAspect = (float)Engine::getScreenWidth() / (float)Engine::getScreenHeight();
    float canvasAspect = (float)Engine::getPreferedCanvasWidth() / (float)Engine::getPreferedCanvasHeight();
    
    //When canvas size is not changed
    if (Engine::getScalingMode() == S_SCALING_LETTERBOX){
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
    
    if (Engine::getScalingMode() == S_SCALING_CROP){
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

}

void Engine::onDraw() {
    
    if (Engine::getScene() != NULL){
        (Engine::getScene())->draw();
    }
    
    auto now = std::chrono::steady_clock::now();
    unsigned long newTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    deltatime = (unsigned int)(newTime - lastTime);
    lastTime = newTime;
    framerate = 1 / (float)deltatime * 1000;
    
    int updateLoops = 0;
    updateTimeCount += deltatime;
    while (updateTimeCount >= updateTime && updateLoops <= 5){
        updateLoops++;
        updateTimeCount -= updateTime;
        
        Events::call_onUpdate();
    }
    
    Events::call_onDraw();
    
    SoundManager::checkActive();
    
}

void Engine::onPause(){
    SoundManager::pauseAll();
}

void Engine::onResume(){
    SoundManager::resumeAll();
}

bool Engine::transformCoordPos(float& x, float& y){
    x = (x * (float)screenWidth / viewRect.getWidth());
    y = (y * (float)screenHeight / viewRect.getHeight());
    
    x = ((float)Engine::getCanvasWidth() * (x+1)) / 2;
    y = ((float)Engine::getCanvasHeight() * (y+1)) / 2;
    
    return ((x >= 0) && (x <= Engine::getCanvasWidth()) && (y >= 0) && (y <= Engine::getCanvasHeight()));
}

void Engine::onTouchPress(float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onTouchPress(x, y);
    }
}

void Engine::onTouchUp(float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onTouchUp(x, y);
    }
}

void Engine::onTouchDrag(float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onTouchDrag(x, y);
    }
}

void Engine::onMousePress(int button, float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onMousePress(button, x, y);
        if ((Engine::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
            Events::call_onTouchPress(x, y);
        }
    }
}
void Engine::onMouseUp(int button, float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onMouseUp(button, x, y);
        if ((Engine::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
            Events::call_onTouchUp(x, y);
        }
    }
}

void Engine::onMouseDrag(int button, float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onMouseDrag(button, x, y);
        if ((Engine::isMouseAsTouch()) && (button == S_MOUSE_BUTTON_LEFT)){
            Events::call_onTouchDrag(x, y);
        }
    }
}

void Engine::onMouseMove(float x, float y){
    if (transformCoordPos(x, y)){
        Events::call_onMouseMove(x, y);
    }
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
