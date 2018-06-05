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

#include "math/Rect.h"
#include "Log.h"
#include "Button.h"

#include "LuaBind.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//#include "Mesh.h"

#include "audio/SoundManager.h"
#include "Input.h"


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
int Engine::scalingMode;
bool Engine::defaultNearestScaleTexture;
bool Engine::defaultResampleToPOTTexture;
bool Engine::fixedTimePhysics;

unsigned long Engine::lastTime = 0;
unsigned int Engine::updateTimeCount = 0;

unsigned int Engine::deltatime = 0;
float Engine::framerate = 0;

unsigned int Engine::updateTime = 30;

//-----Supernova user events-----
void (*Engine::onCanvasLoadedFunc)();
int Engine::onCanvasLoadedLuaFunc;

void (*Engine::onCanvasChangedFunc)();
int Engine::onCanvasChangedLuaFunc;

void (*Engine::onDrawFunc)();
int Engine::onDrawLuaFunc;

void (*Engine::onUpdateFunc)();
int Engine::onUpdateLuaFunc;

void (*Engine::onTouchStartFunc)(int, float, float);
int Engine::onTouchStartLuaFunc;

void (*Engine::onTouchEndFunc)(int, float, float);
int Engine::onTouchEndLuaFunc;

void (*Engine::onTouchDragFunc)(int, float, float);
int Engine::onTouchDragLuaFunc;

void (*Engine::onMouseDownFunc)(int, float, float);
int Engine::onMouseDownLuaFunc;

void (*Engine::onMouseUpFunc)(int, float, float);
int Engine::onMouseUpLuaFunc;

void (*Engine::onMouseDragFunc)(int, float, float);
int Engine::onMouseDragLuaFunc;

void (*Engine::onMouseMoveFunc)(float, float);
int Engine::onMouseMoveLuaFunc;

void (*Engine::onKeyDownFunc)(int);
int Engine::onKeyDownLuaFunc;

void (*Engine::onKeyUpFunc)(int);
int Engine::onKeyUpLuaFunc;


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

void Engine::setFixedTimePhysics(bool fixedTimePhysics){
    Engine::fixedTimePhysics = fixedTimePhysics;
}

bool Engine::isFixedTimePhysics(){
    return fixedTimePhysics;
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

void Engine::systemStart(){

    systemStart(0, 0);

}

void Engine::systemStart(int width, int height){

    Engine::setScreenSize(width, height);

    Engine::setMouseAsTouch(true);
    Engine::setUseDegrees(true);
    Engine::setRenderAPI(S_GLES2);
    Engine::setScalingMode(S_SCALING_FITWIDTH);
    Engine::setDefaultNearestScaleTexture(false);
    Engine::setDefaultResampleToPOTTexture(true);
    Engine::setFixedTimePhysics(false);
    
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

    call_onCanvasLoaded();
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

    call_onCanvasChanged();
}

void Engine::systemDraw() {

    if (!fixedTimePhysics && Engine::getScene())
        (Engine::getScene())->updatePhysics(Engine::getDeltatime() / 1000.0f);
    
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

        if (fixedTimePhysics && Engine::getScene())
            (Engine::getScene())->updatePhysics(updateTime / 1000.0f);

        Engine::call_onUpdate();
    }

    Engine::call_onDraw();

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
        Engine::call_onTouchStart(pointer, x, y);

        if (mainScene) {
            std::vector<GUIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engine_onDown(pointer, x, y);
            }
        }
    }
}

void Engine::systemTouchEnd(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        Engine::call_onTouchEnd(pointer, x, y);

        if (mainScene) {
            std::vector<GUIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engine_onUp(pointer, x, y);
            }
        }
    }
}

void Engine::systemTouchDrag(int pointer, float x, float y){
    if (transformCoordPos(x, y)){
        Engine::call_onTouchDrag(pointer, x, y);
    }
}

void Engine::systemMouseDown(int button, float x, float y){
    if (transformCoordPos(x, y)){
        Engine::call_onMouseDown(button, x, y);
        if (Engine::isMouseAsTouch()){
            Engine::call_onTouchStart(button, x, y);
        }

        if (mainScene) {
            std::vector<GUIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engine_onDown(button, x, y);
            }
        }
    }
}
void Engine::systemMouseUp(int button, float x, float y){
    if (transformCoordPos(x, y)){
        Engine::call_onMouseUp(button, x, y);
        if (Engine::isMouseAsTouch()){
            Engine::call_onTouchEnd(button, x, y);
        }

        if (mainScene) {
            std::vector<GUIObject *>::iterator it;
            for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
                (*it)->engine_onUp(button, x, y);
            }
        }
    }
}

void Engine::systemMouseDrag(int button, float x, float y){
    if (transformCoordPos(x, y)){
        Engine::call_onMouseDrag(button, x, y);
        if (Engine::isMouseAsTouch()){
            Engine::call_onTouchDrag(button, x, y);
        }
    }
}

void Engine::systemMouseMove(float x, float y){
    if (transformCoordPos(x, y)){
        Engine::call_onMouseMove(x, y);
    }
}

void Engine::systemKeyDown(int inputKey){
    Engine::call_onKeyDown(inputKey);
    //printf("keypress %i\n", inputKey);
}

void Engine::systemKeyUp(int inputKey){
    Engine::call_onKeyUp(inputKey);
    //printf("keyup %i\n", inputKey);
}

void Engine::systemTextInput(const char* text){
    //Log::Verbose("textinput %s\n", text);
    if (mainScene) {
        std::vector<GUIObject*>::iterator it;
        for (it = mainScene->guiObjects.begin(); it != mainScene->guiObjects.end(); ++it) {
            (*it)->engine_onTextInput(text);
        }
    }
}

void Engine::onCanvasLoaded(void (*onCanvasLoadedFunc)()){
    Engine::onCanvasLoadedFunc = onCanvasLoadedFunc;
}

int Engine::onCanvasLoaded(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onCanvasLoadedLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onCanvasLoaded is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onCanvasLoaded(){
    if (onCanvasLoadedFunc != NULL){
        onCanvasLoadedFunc();
    }
    if (onCanvasLoadedLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onCanvasLoadedLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void Engine::onCanvasChanged(void (*onCanvasChangedFunc)()){
    Engine::onCanvasChangedFunc = onCanvasChangedFunc;
}

int Engine::onCanvasChanged(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onCanvasChangedLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onCanvasChanged is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onCanvasChanged(){
    if (onCanvasChangedFunc != NULL){
        onCanvasChangedFunc();
    }
    if (onCanvasChangedLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onCanvasChangedLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void Engine::onDraw(void (*onDrawFunc)()){
    Engine::onDrawFunc = onDrawFunc;
}

int Engine::onDraw(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onDrawLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onDraw is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onDraw(){
    if (onDrawFunc != NULL){
        onDrawFunc();
    }
    if (onDrawLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onDrawLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void Engine::onUpdate(void (*onUpdateFunc)()){
    Engine::onUpdateFunc = onUpdateFunc;
}

int Engine::onUpdate(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onUpdateLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onUpdate is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onUpdate(){
    if (onUpdateFunc != NULL){
        onUpdateFunc();
    }
    if (onUpdateLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onUpdateLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void Engine::onTouchStart(void (*onTouchStartFunc)(int, float, float)){
    Engine::onTouchStartFunc = onTouchStartFunc;
}

int Engine::onTouchStart(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onTouchStartLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onTouchStart is not lua function");
        return luaL_error(L, "This is not a valid function");

    }
    return 0;
}

void Engine::call_onTouchStart(int pointer, float x, float y){
    if (onTouchStartFunc != NULL){
        onTouchStartFunc(pointer, x, y);
    }
    if (onTouchStartLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onTouchStartLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), pointer);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::addTouchStarted();
    Input::setTouchPosition(x, y);
}

void Engine::onTouchEnd(void (*onTouchEndFunc)(int, float, float)){
    Engine::onTouchEndFunc = onTouchEndFunc;
}

int Engine::onTouchEnd(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onTouchEndLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onTouchEnd is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onTouchEnd(int pointer, float x, float y){
    if (onTouchEndFunc != NULL){
        onTouchEndFunc(pointer, x, y);
    }
    if (onTouchEndLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onTouchEndLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), pointer);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::releaseTouchStarted();
    Input::setTouchPosition(x, y);
}

void Engine::onTouchDrag(void (*onTouchDragFunc)(int, float, float)){
    Engine::onTouchDragFunc = onTouchDragFunc;
}

int Engine::onTouchDrag(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onTouchDragLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onTouchDrag is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onTouchDrag(int pointer, float x, float y){
    if (onTouchDragFunc != NULL){
        onTouchDragFunc(pointer, x, y);
    }
    if (onTouchDragLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onTouchDragLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), pointer);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::setTouchPosition(x, y);
}

void Engine::onMouseDown(void (*onMouseDownFunc)(int, float, float)){
    Engine::onMouseDownFunc = onMouseDownFunc;
}

int Engine::onMouseDown(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onMouseDownLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onMouseDown is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onMouseDown(int button, float x, float y){
    if (onMouseDownFunc != NULL){
        onMouseDownFunc(button, x, y);
    }
    if (onMouseDownLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onMouseDownLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), button);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::addMousePressed(button);
    Input::setMousePosition(x, y);
}

void Engine::onMouseUp(void (*onMouseUpFunc)(int, float, float)){
    Engine::onMouseUpFunc = onMouseUpFunc;
}

int Engine::onMouseUp(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onMouseUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onMouseUp is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onMouseUp(int button, float x, float y){
    if (onMouseUpFunc != NULL){
        onMouseUpFunc(button, x, y);
    }
    if (onMouseUpLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onMouseUpLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), button);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::releaseMousePressed(button);
    Input::setMousePosition(x, y);
}

void Engine::onMouseDrag(void (*onMouseDragFunc)(int, float, float)){
    Engine::onMouseDragFunc = onMouseDragFunc;
}

int Engine::onMouseDrag(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onMouseDragLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onMouseDrag is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onMouseDrag(int button, float x, float y){
    if (onMouseDragFunc != NULL){
        onMouseDragFunc(button, x, y);
    }
    if (onMouseDragLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onMouseDragLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), button);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::setMousePosition(x, y);
}

void Engine::onMouseMove(void (*onMouseMoveFunc)(float, float)){
    Engine::onMouseMoveFunc = onMouseMoveFunc;
}

int Engine::onMouseMove(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onMouseMoveLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onMouseMove is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onMouseMove(float x, float y){
    if (onMouseMoveFunc != NULL){
        onMouseMoveFunc(x, y);
    }
    if (onMouseMoveLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onMouseMoveLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(2, 0, 0);
    }
    Input::setMousePosition(x, y);
}

void Engine::onKeyDown(void (*onKeyDownFunc)(int)){
    Engine::onKeyDownFunc = onKeyDownFunc;
}

int Engine::onKeyDown(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onKeyDownLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onKeyDown is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onKeyDown(int key){
    if (onKeyDownFunc != NULL){
        onKeyDownFunc(key);
    }
    if (onKeyDownLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onKeyDownLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), key);
        LuaBind::luaCallback(1, 0, 0);
    }
    Input::addKeyPressed(key);
}

void Engine::onKeyUp(void (*onKeyUpFunc)(int)){
    Engine::onKeyUpFunc = onKeyUpFunc;
}

int Engine::onKeyUp(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Engine::onKeyUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting onKeyUp is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void Engine::call_onKeyUp(int key){
    if (onKeyUpFunc != NULL){
        onKeyUpFunc(key);
    }
    if (onKeyUpLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Engine::onKeyUpLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), key);
        LuaBind::luaCallback(1, 0, 0);
    }
    Input::releaseKeyPressed(key);
}
