//
// (c) 2023 Eduardo Doria.
//

#include "Engine.h"
#include "Scene.h"
#include "System.h"
#include "Input.h"
#include "render/SystemRender.h"
#include "script/LuaBinding.h"
#include "subsystem/AudioSystem.h"
#include "subsystem/RenderSystem.h"
#include "subsystem/UISystem.h"

#include "sokol_time.h"

using namespace Supernova;

//-----Supernova user config-----
Scene* Engine::scenes[MAX_SCENE_LAYERS] = {NULL};
size_t Engine::numScenes = 0;

int Engine::canvasWidth;
int Engine::canvasHeight;

int Engine::preferedCanvasWidth;
int Engine::preferedCanvasHeight;

Rect Engine::viewRect;

Scaling Engine::scalingMode;
TextureStrategy Engine::textureStrategy;

bool Engine::callMouseInTouchEvent;
bool Engine::callTouchInMouseEvent;
bool Engine::useDegrees;
bool Engine::automaticTransparency;
bool Engine::allowEventsOutCanvas;
bool Engine::fixedTimeSceneUpdate;

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
FunctionSubscribe<void(int,float,float,int)> Engine::onMouseDown;
FunctionSubscribe<void(int,float,float,int)> Engine::onMouseUp;
FunctionSubscribe<void(float,float,int)> Engine::onMouseScroll;
FunctionSubscribe<void(float,float,int)> Engine::onMouseMove;
FunctionSubscribe<void()> Engine::onMouseEnter;
FunctionSubscribe<void()> Engine::onMouseLeave;
FunctionSubscribe<void(int,bool,int)> Engine::onKeyDown;
FunctionSubscribe<void(int,bool,int)> Engine::onKeyUp;
FunctionSubscribe<void(wchar_t)> Engine::onCharInput;


void Engine::setScene(Scene* scene){
    if (scene){
        if (!Engine::scenes[0]){
            numScenes++;
        }
        Engine::scenes[0] = scene;
        scene->setMainScene(true);
    }
}

Scene* Engine::getScene(){
    return Engine::scenes[0];
}

void Engine::addSceneLayer(Scene* scene){
    bool foundSlot = false;
    // 0 is reserved to mainScene
    for (int i = 1; i < MAX_SCENE_LAYERS; i++){
        if (!scenes[i] && !foundSlot){
            scenes[i] = scene;
            scenes[i]->setMainScene(false);

            numScenes++;
            foundSlot = true;
        }
    }
    if (!foundSlot){
        Log::error("Scene layers is full. Max scenes is: %i", MAX_SCENE_LAYERS);
    }
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

void Engine::setTextureStrategy(TextureStrategy textureStrategy){
    Engine::textureStrategy = textureStrategy;
}

TextureStrategy Engine::getTextureStrategy(){
    return textureStrategy;
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

void Engine::setUpdateTimeMS(unsigned int updateTimeMS){
    Engine::updateTime = updateTimeMS / 1000.0f;
}

void Engine::setUpdateTime(float updateTime){
    Engine::updateTime = updateTime;
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
    #if TARGET_OS_SIMULATOR
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

bool Engine::isOpenGL(){
    GraphicBackend gbackend = getGraphicBackend();

    if (gbackend == GraphicBackend::GLCORE33)
        return true;
    if (gbackend == GraphicBackend::GLES2)
        return true;
    if (gbackend == GraphicBackend::GLES3)
        return true;

    return false;
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
        if (scalingMode == Scaling::NATIVE){
            Engine::canvasHeight = screenHeight;
            Engine::canvasWidth = screenWidth;
        }
    }
}

void Engine::systemInit(int argc, char* argv[]){

    Engine::setCanvasSize(1000,480);
    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setTextureStrategy(TextureStrategy::RESAMPLE);
    Engine::setCallMouseInTouchEvent(false);
    Engine::setCallTouchInMouseEvent(false);
    Engine::setUseDegrees(true);
    Engine::setAutomaticTransparency(true);
    Engine::setAllowEventsOutCanvas(false);
    Engine::setFixedTimeSceneUpdate(false);

    stm_setup();
    
    std::vector<std::string> args(argv, argv + argc);
    System::instance().args = args;

    LuaBinding::createLuaState();

    #ifndef NO_LUA_INIT
    LuaBinding::init();
    #endif
    #ifndef NO_CPP_INIT
    init();
    #endif
}

void Engine::systemViewLoaded(){
    SystemRender::setup();
    onViewLoaded.call();
    for (int i = 0; i < numScenes; i++){
        scenes[i]->load();
    }
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

    for (int i = 0; i < numScenes; i++){
        scenes[i]->getSystem<RenderSystem>()->updateCameraSize(scenes[i]->getCamera());
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

        if (isFixedTimeSceneUpdate()){
            for (int i = 0; i < numScenes; i++){
                scenes[i]->update(updateTime);
            }
        }
    }
    if (updateLoops > 100){
        Log::warn("More than 100 updates in a frame");
    }

    if (!isFixedTimeSceneUpdate()){
        for (int i = 0; i < numScenes; i++){
            scenes[i]->update(deltatime);
        }
    }

    Engine::onDraw.call();

    for (int i = 0; i < numScenes; i++){
        scenes[i]->draw();
    }
    
    SystemRender::commit();
    AudioSystem::checkActive();
}

void Engine::systemShutdown(){
    for (int i = 0; i < numScenes; i++){
        scenes[i]->destroy();
    }
    SystemRender::shutdown();
    Engine::onShutdown.call();
}

void Engine::systemPause(){
    AudioSystem::pauseAll();
}

void Engine::systemResume(){
    AudioSystem::resumeAll();
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
            Engine::onMouseDown.call(S_MOUSE_BUTTON_1, x, y, 0);
            Input::addMousePressed(S_MOUSE_BUTTON_1);
            Input::setMousePosition(x, y);
            //-----------------
        }

        for (int i = 0; i < numScenes; i++){
            if (scenes[i]->isEnableUIEvents())
                scenes[i]->getSystem<UISystem>()->eventOnPointerDown(x, y);
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
            Engine::onMouseUp.call(S_MOUSE_BUTTON_1, x, y, 0);
            Input::releaseMousePressed(S_MOUSE_BUTTON_1);
            Input::setMousePosition(x, y);
            //-----------------
        }

        for (int i = 0; i < numScenes; i++){
            if (scenes[i]->isEnableUIEvents())
                scenes[i]->getSystem<UISystem>()->eventOnPointerUp(x, y);
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
            Engine::onMouseMove.call(x, y, 0);
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
        Engine::onMouseDown.call(button, x, y, mods);
        Input::addMousePressed(button);
        Input::setMousePosition(x, y);
        if (mods != 0)
            Input::setModifiers(mods);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchStart.call(0, x, y);
            Input::addTouch(0, x, y);
            //-----------------
        }

        for (int i = 0; i < numScenes; i++){
            if (scenes[i]->isEnableUIEvents())
                if (button == S_MOUSE_BUTTON_1)
                    scenes[i]->getSystem<UISystem>()->eventOnPointerDown(x, y);
        }
    }
}
void Engine::systemMouseUp(int button, float x, float y, int mods){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseUp.call(button, x, y, mods);
        Input::releaseMousePressed(button);
        Input::setMousePosition(x, y);
        if (mods != 0)
            Input::setModifiers(mods);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            Engine::onTouchEnd.call(0, x, y);
            Input::removeTouch(0);
            //-----------------
        }

        for (int i = 0; i < numScenes; i++){
            if (scenes[i]->isEnableUIEvents())
                if (button == S_MOUSE_BUTTON_1)
                    scenes[i]->getSystem<UISystem>()->eventOnPointerUp(x, y);
        }
    }
}

void Engine::systemMouseMove(float x, float y, int mods){
    if (transformCoordPos(x, y)){
        //-----------------
        Engine::onMouseMove.call(x, y, mods);
        Input::setMousePosition(x, y);
        if (mods != 0)
            Input::setModifiers(mods);
        //-----------------
        if (Engine::isCallTouchInMouseEvent()){
            //-----------------
            if (Input::isMousePressed(S_MOUSE_BUTTON_LEFT) || Input::isMousePressed(S_MOUSE_BUTTON_RIGHT)){
                Engine::onTouchMove.call(0, x, y);
                Input::setTouchPosition(0, x, y);
            }
            //-----------------
        }
    }
}

void Engine::systemMouseScroll(float xoffset, float yoffset, int mods){
    //-----------------
    Engine::onMouseScroll.call(xoffset, yoffset, mods);
    Input::setMouseScroll(xoffset, yoffset);
    if (mods != 0)
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
    if (mods != 0)
        Input::setModifiers(mods);
    //-----------------
}

void Engine::systemKeyUp(int key, bool repeat, int mods){
    //-----------------
    Engine::onKeyUp.call(key, repeat, mods);
    Input::releaseKeyPressed(key);
    Input::setModifiers(mods); // Now it can be 0
    //-----------------
}

void Engine::systemCharInput(wchar_t codepoint){
    onCharInput.call(codepoint);

    for (int i = 0; i < numScenes; i++){
        if (scenes[i]->isEnableUIEvents())
            scenes[i]->getSystem<UISystem>()->eventOnCharInput(codepoint);
    }
}
