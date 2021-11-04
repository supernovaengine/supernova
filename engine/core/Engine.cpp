//
// (c) 2021 Eduardo Doria.
//

#include "Engine.h"
#include "Scene.h"
#include "Supernova.h"
#include "System.h"
#include "Input.h"
#include "render/SystemRender.h"
#include "script/LuaBinding.h"

#include "sokol_time.h"

using namespace Supernova;

//-----Supernova user config-----
Scene *Engine::mainScene = NULL;

int Engine::canvasWidth;
int Engine::canvasHeight;

int Engine::preferedCanvasWidth;
int Engine::preferedCanvasHeight;

Rect Engine::viewRect;

Scaling Engine::scalingMode;

bool Engine::callMouseInTouchEvent;
bool Engine::callTouchInMouseEvent;
bool Engine::useDegrees;
bool Engine::defaultNearestScaleTexture;
bool Engine::defaultResampleToPOTTexture;
bool Engine::automaticTransparency;
bool Engine::allowEventsOutCanvas;
bool Engine::fixedTimeSceneUpdate;
bool Engine::fixedTimePhysics;
bool Engine::fixedTimeAnimations;

uint64_t Engine::lastTime = 0;
float Engine::updateTimeCount = 0;

double Engine::deltatime = 0;
float Engine::framerate = 0;

float Engine::updateTime = 0.03;

//-----Supernova user events-----
FunctionSubscribe<void()> Engine::onViewLoaded;
FunctionSubscribe<void()> Engine::onViewChanged;
FunctionSubscribe<void()> Engine::onDraw;
FunctionSubscribe<void()> Engine::onUpdate;
FunctionSubscribe<void()> Engine::onShutdown;
FunctionSubscribe<void(int,float,float)> Engine::onTouchStart;
FunctionSubscribe<void(int,float,float)> Engine::onTouchEnd;
FunctionSubscribe<void(int,float,float)> Engine::onTouchMove;
FunctionSubscribe<void()> Engine::onTouchCancel;
FunctionSubscribe<void(int,int)> Engine::onMouseDown;
FunctionSubscribe<void(int,int)> Engine::onMouseUp;
FunctionSubscribe<void(float,float)> Engine::onMouseScroll;
FunctionSubscribe<void(float,float)> Engine::onMouseMove;
FunctionSubscribe<void()> Engine::onMouseEnter;
FunctionSubscribe<void()> Engine::onMouseLeave;
FunctionSubscribe<void(int,bool,int)> Engine::onKeyDown;
FunctionSubscribe<void(int,bool,int)> Engine::onKeyUp;
FunctionSubscribe<void(wchar_t)> Engine::onCharInput;


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

int Engine::getPreferedCanvasWidth(){
    return Engine::preferedCanvasWidth;
}

int Engine::getPreferedCanvasHeight(){
    return Engine::preferedCanvasHeight;
}

Rect Engine::getViewRect(){
    return viewRect;
}

void Engine::setScalingMode(Scaling scalingMode){
    Engine::scalingMode = scalingMode;
}

Scaling Engine::getScalingMode(){
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

void Engine::setAutomaticTransparency(bool automaticTransparency){
    Engine::automaticTransparency = automaticTransparency;
}

bool Engine::isAutomaticTransparency(){
    return automaticTransparency;
}

void Engine::setAllowEventsOutCanvas(bool allowEventsOutCanvas){
    Engine::allowEventsOutCanvas = allowEventsOutCanvas;
}

bool Engine::isAllowEventsOutCanvas(){
    return allowEventsOutCanvas;
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

Platform Engine::getPlatform(){
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    return Platform::Windows;
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
    return Platform::iOS;
    #elif TARGET_OS_IPHONE
    return Platform::iOS;
    #elif TARGET_OS_MAC
    return Platform::MacOS;
    #endif
#elif __ANDROID__
    return Platform::Android;
#elif __EMSCRIPTEN__
    return Platform::Web;
#elif __linux__
    return Platform::Linux;
#endif
}

GraphicBackend Engine::getGraphicBackend(){
#if defined(SOKOL_GLCORE33)
    return GraphicBackend::GLCORE33;
#elif defined(SOKOL_GLES2)
    return GraphicBackend::GLES2;
#elif defined(SOKOL_GLES3)
    return GraphicBackend::GLES3;
#elif defined(SOKOL_D3D11)
    return GraphicBackend::D3D11;
#elif defined(SOKOL_METAL)
    return GraphicBackend::METAL;
#elif defined(SOKOL_WGPU)
    return GraphicBackend::WGPU;
#elif defined(SUPERNOVA_APPLE) //Xcode template
    return GraphicBackend::METAL;
#endif
}

float Engine::getFramerate(){
    return framerate;
}

float Engine::getDeltatime(){
    return deltatime;
}

void Engine::calculateCanvas(){
    Engine::canvasWidth = preferedCanvasWidth;
    Engine::canvasHeight = preferedCanvasHeight;

    int screenWidth = System::instance().getScreenWidth();
    int screenHeight = System::instance().getScreenHeight();

    //When canvas size is changed
    if (screenWidth != 0 && screenHeight != 0){
        if (scalingMode == Scaling::FITWIDTH){
            Engine::canvasWidth = preferedCanvasWidth;
            Engine::canvasHeight = screenHeight * preferedCanvasWidth / screenWidth;
        }
        if (scalingMode == Scaling::FITHEIGHT){
            Engine::canvasHeight = preferedCanvasHeight;
            Engine::canvasWidth = screenWidth * preferedCanvasHeight / screenHeight;
        }
    }
}

void Engine::systemInit(int argc, char* argv[]){

    Engine::setCanvasSize(1000,480);
    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setCallMouseInTouchEvent(false);
    Engine::setCallTouchInMouseEvent(false);
    Engine::setUseDegrees(true);
    Engine::setDefaultNearestScaleTexture(false);
    Engine::setDefaultResampleToPOTTexture(true);
    Engine::setAutomaticTransparency(true);
    Engine::setAllowEventsOutCanvas(false);
    Engine::setFixedTimeSceneUpdate(false);

    stm_setup();
    
    std::vector<std::string> args(argv, argv + argc);
    System::instance().args = args;

    LuaBinding::createLuaState();
    LuaBinding::bind();

    #ifndef NO_CPP_INIT
    init();
    #endif
}

void Engine::systemViewLoaded(){
    SystemRender::setup();
    onViewLoaded.call();
    if (getScene())
        mainScene->load();
}

void Engine::systemViewChanged(){
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
        Engine::getScene()->updateCameraSize();
    }

    onViewChanged.call();
}

void Engine::systemDraw(){
    //Deltatime in seconds
    deltatime = stm_sec(stm_laptime(&lastTime));
    framerate = 1 / deltatime;

    int updateLoops = 0;
    updateTimeCount += deltatime;
    while (updateTimeCount >= updateTime && updateLoops <= 100){
        updateLoops++;
        updateTimeCount -= updateTime;

        Engine::onUpdate.call();

        if (isFixedTimeSceneUpdate() && Engine::getScene())
            mainScene->update(updateTime);
    }
    if (updateLoops > 100){
        Log::Warn("More than 100 updates in a frame");
    }

    if (!isFixedTimeSceneUpdate() && Engine::getScene()){
        mainScene->update(deltatime);
    }

    Engine::onDraw.call();

    if (Engine::getScene())
        mainScene->draw();
    
    //SoundManager::checkActive();
}

void Engine::systemShutdown(){
    SystemRender::shutdown();
    Engine::onShutdown.call();
}

void Engine::systemPause(){
    //SoundManager::pauseAll();
}

void Engine::systemResume(){
    //SoundManager::resumeAll();
}

bool Engine::transformCoordPos(float& x, float& y){

    x = (x - viewRect.getX()) / viewRect.getWidth();
    y = (y - viewRect.getY()) / viewRect.getHeight();
    
    x = (float)Engine::getCanvasWidth() * x;
    y = (float)Engine::getCanvasHeight() * y;
    
    if (allowEventsOutCanvas)
        return true;
    
    return ((x >= 0) && (x <= Engine::getCanvasWidth()) && (y >= 0) && (y <= Engine::getCanvasHeight()));
}

void Engine::systemTouchStart(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onTouchStart.call(pointer, x, y);
        Input::addTouch(pointer, x, y);
        //-----------------
        if (Engine::isCallMouseInTouchEvent()){
            //-----------------
            Engine::onMouseDown.call(100, 0);
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
        Input::removeTouch(pointer);
        //-----------------
        if (Engine::isCallMouseInTouchEvent()){
            //-----------------
            Engine::onMouseUp.call(100, 0);
            Input::releaseMousePressed(100);
            Input::setMousePosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemTouchMove(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onTouchMove.call(pointer, x, y);
        Input::setTouchPosition(pointer, x, y);
        //-----------------
        if (Engine::isCallMouseInTouchEvent()){
            //-----------------
            Engine::onMouseMove.call(x, y);
            Input::setMousePosition(x, y);
            //-----------------
        }
    }
}

void Engine::systemTouchCancel(){
    //-----------------
    Engine::onTouchCancel.call();
    Input::clearTouches();
    //-----------------
}

void Engine::systemMouseDown(int button, float x, float y, int mods){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseDown.call(button, mods);
        Input::addMousePressed(button);
        Input::setMousePosition(x, y);
        Input::setModifiers(mods);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchStart.call(100, x, y);
            Input::addTouch(100, x, y);
            //-----------------
        }
    }
}
void Engine::systemMouseUp(int button, float x, float y, int mods){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseUp.call(button, mods);
        Input::releaseMousePressed(button);
        Input::setMousePosition(x, y);
        Input::setModifiers(mods);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchEnd.call(100, x, y);
            Input::removeTouch(100);
            //-----------------
        }
    }
}

void Engine::systemMouseMove(float x, float y, int mods){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseMove.call(x, y);
        Input::setMousePosition(x, y);
        Input::setModifiers(mods);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            if (Input::isMousePressed(S_MOUSE_BUTTON_LEFT) || Input::isMousePressed(S_MOUSE_BUTTON_RIGHT)){
                Engine::onTouchMove.call(100, x, y);
                Input::setTouchPosition(100, x, y);
            }
            //-----------------
        }
    }
}

void Engine::systemMouseScroll(float xoffset, float yoffset, int mods){
    //-----------------
    Engine::onMouseScroll.call(xoffset, yoffset);
    Input::setMouseScroll(xoffset, yoffset);
    Input::setModifiers(mods);
    //-----------------
}

void Engine::systemMouseEnter(){
    //-----------------
    Engine::onMouseEnter.call();
    Input::addMouseEntered();
    //-----------------
}

void Engine::systemMouseLeave(){
    //-----------------
    Engine::onMouseLeave.call();
    Input::releaseMouseEntered();
    //-----------------
}

void Engine::systemKeyDown(int key, bool repeat, int mods){
    //-----------------
    Engine::onKeyDown.call(key, repeat, mods);
    Input::addKeyPressed(key);
    //-----------------
}

void Engine::systemKeyUp(int key, bool repeat, int mods){
    //-----------------
    Engine::onKeyUp.call(key, repeat, mods);
    Input::releaseKeyPressed(key);
    //-----------------
}

void Engine::systemCharInput(wchar_t codepoint){
    onCharInput.call(codepoint);
}
