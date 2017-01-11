#include "Supernova.h"
#include "platform/Log.h"

lua_State *Supernova::luastate;
Scene *Supernova::mainScene;

int Supernova::screenWidth;
int Supernova::screenHeight;

int Supernova::canvasWidth;
int Supernova::canvasHeight;

int Supernova::preferedCanvasWidth;
int Supernova::preferedCanvasHeight;

void (*Supernova::onFrameFunc)();
int Supernova::onFrameLuaFunc;

void (*Supernova::onTouchPressFunc)(float, float);
int Supernova::onTouchPressLuaFunc;

void (*Supernova::onTouchUpFunc)(float, float);
int Supernova::onTouchUpLuaFunc;

void (*Supernova::onTouchDragFunc)(float, float);
int Supernova::onTouchDragLuaFunc;

void (*Supernova::onMousePressFunc)(int, float, float);
int Supernova::onMousePressLuaFunc;

void (*Supernova::onMouseUpFunc)(int, float, float);
int Supernova::onMouseUpLuaFunc;

void (*Supernova::onMouseDragFunc)(int, float, float);
int Supernova::onMouseDragLuaFunc;

void (*Supernova::onMouseMoveFunc)(float, float);
int Supernova::onMouseMoveLuaFunc;

void (*Supernova::onKeyPressFunc)(int);
int Supernova::onKeyPressLuaFunc;

void (*Supernova::onKeyUpFunc)(int);
int Supernova::onKeyUpLuaFunc;

int Supernova::renderAPI;
bool Supernova::mouseAsTouch;
bool Supernova::useDegrees;
int Supernova::scalingMode;


Supernova::Supernova() {
	// TODO Auto-generated constructor stub

}

Supernova::~Supernova() {
	// TODO Auto-generated destructor stub
}

void Supernova::luaCallback(int nargs, int nresults, int msgh){
    int status = lua_pcall(luastate, nargs, nresults, msgh);
    if (status != 0){
        Log::Error(LOG_TAG, "Lua Error: %s\n", lua_tostring(luastate,-1));
    }
}

void Supernova::setLuaState(lua_State* luastate){
    Supernova::luastate = luastate;
}

lua_State* Supernova::getLuaState(){
	return luastate;
}

void Supernova::setScene(Scene *mainScene){
	Supernova::mainScene = mainScene;
}

Scene* Supernova::getScene(){
	return mainScene;
}

int Supernova::getScreenWidth(){
    return Supernova::screenWidth;
}

int Supernova::getScreenHeight(){
    return Supernova::screenHeight;
}

void Supernova::setScreenSize(int screenWidth, int screenHeight){

    Supernova::screenWidth = screenWidth;
    Supernova::screenHeight = screenHeight;

    if ((Supernova::preferedCanvasWidth != 0) && (Supernova::preferedCanvasHeight != 0)){
        setCanvasSize(preferedCanvasWidth, preferedCanvasHeight);
    }

}

int Supernova::getCanvasWidth(){
    return Supernova::canvasWidth;
}

int Supernova::getCanvasHeight(){
    return Supernova::canvasHeight;
}

void Supernova::setCanvasSize(int canvasWidth, int canvasHeight){
    
    Supernova::canvasWidth = canvasWidth;
    Supernova::canvasHeight = canvasHeight;
    
    if ((Supernova::screenWidth == 0) || (Supernova::screenHeight == 0)){
        setScreenSize(canvasWidth, canvasHeight);
    }
    
    //When canvas size is changed
    if (scalingMode == S_SCALING_FITWIDTH){
        Supernova::canvasWidth = canvasWidth;
        Supernova::canvasHeight = screenHeight * canvasWidth / screenWidth;
    }
    if (scalingMode == S_SCALING_FITHEIGHT){
        Supernova::canvasHeight = canvasHeight;
        Supernova::canvasWidth = screenWidth * canvasHeight / screenHeight;
    }
    // S_SCALING_STRETCH do not need nothing
    
    if ((Supernova::preferedCanvasWidth == 0) && (Supernova::preferedCanvasHeight == 0)){
        setPreferedCanvasSize(canvasWidth, canvasHeight);
    }

}

int Supernova::getPreferedCanvasWidth(){
    return Supernova::preferedCanvasWidth;
}

int Supernova::getPreferedCanvasHeight(){
    return Supernova::preferedCanvasHeight;
}

void Supernova::setPreferedCanvasSize(int preferedCanvasWidth, int preferedCanvasHeight){
    if ((Supernova::preferedCanvasWidth == 0) && (Supernova::preferedCanvasHeight == 0)){
        Supernova::preferedCanvasWidth = preferedCanvasWidth;
        Supernova::preferedCanvasHeight = preferedCanvasHeight;
    }
}

void Supernova::setRenderAPI(int renderAPI){
    Supernova::renderAPI = renderAPI;
}

int Supernova::getRenderAPI(){
    return renderAPI;
}

void Supernova::setScalingMode(int scalingMode){
    Supernova::scalingMode = scalingMode;
}

int Supernova::getScalingMode(){
    return scalingMode;
}

void Supernova::setMouseAsTouch(bool mouseAsTouch){
    Supernova::mouseAsTouch = mouseAsTouch;
}

bool Supernova::isMouseAsTouch(){
    return Supernova::mouseAsTouch;
}

void Supernova::setUseDegrees(bool useDegrees){
    Supernova::useDegrees = useDegrees;
}

bool Supernova::isUseDegrees(){
    return Supernova::useDegrees;
}

int Supernova::getPlatform(){
    
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

void Supernova::onFrame(void (*onFrameFunc)()){
    Supernova::onFrameFunc = onFrameFunc;
}

int Supernova::onFrame(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onFrameLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onFrame(){
    if (onFrameFunc != NULL){
        onFrameFunc();
    }
    if (onFrameLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onFrameLuaFunc);
        luaCallback(0, 0, 0);
    }
}

void Supernova::onTouchPress(void (*onTouchPressFunc)(float, float)){
    Supernova::onTouchPressFunc = onTouchPressFunc;
}

int Supernova::onTouchPress(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onTouchPressLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onTouchPress(float x, float y){
    if (onTouchPressFunc != NULL){
        onTouchPressFunc(x, y);
    }
    if (onTouchPressLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onTouchPressLuaFunc);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(2, 0, 0);
    }
}

void Supernova::onTouchUp(void (*onTouchUpFunc)(float, float)){
    Supernova::onTouchUpFunc = onTouchUpFunc;
}

int Supernova::onTouchUp(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onTouchUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onTouchUp(float x, float y){
    if (onTouchUpFunc != NULL){
        onTouchUpFunc(x, y);
    }
    if (onTouchUpLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onTouchUpLuaFunc);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(2, 0, 0);
    }
}

void Supernova::onTouchDrag(void (*onTouchDragFunc)(float, float)){
    Supernova::onTouchDragFunc = onTouchDragFunc;
}

int Supernova::onTouchDrag(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onTouchDragLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onTouchDrag(float x, float y){
    if (onTouchDragFunc != NULL){
        onTouchDragFunc(x, y);
    }
    if (onTouchDragLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onTouchDragLuaFunc);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(2, 0, 0);
    }
}

void Supernova::onMousePress(void (*onMousePressFunc)(int, float, float)){
    Supernova::onMousePressFunc = onMousePressFunc;
}

int Supernova::onMousePress(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onMousePressLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onMousePress(int button, float x, float y){
    if (onMousePressFunc != NULL){
        onMousePressFunc(button, x, y);
    }
    if (onMousePressLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onMousePressLuaFunc);
        lua_pushnumber(luastate, button);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(3, 0, 0);
    }
}

void Supernova::onMouseUp(void (*onMouseUpFunc)(int, float, float)){
    Supernova::onMouseUpFunc = onMouseUpFunc;
}

int Supernova::onMouseUp(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onMouseUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onMouseUp(int button, float x, float y){
    if (onMouseUpFunc != NULL){
        onMouseUpFunc(button, x, y);
    }
    if (onMouseUpLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onMouseUpLuaFunc);
        lua_pushnumber(luastate, button);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(3, 0, 0);
    }
}

void Supernova::onMouseDrag(void (*onMouseDragFunc)(int, float, float)){
    Supernova::onMouseDragFunc = onMouseDragFunc;
}

int Supernova::onMouseDrag(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onMouseDragLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onMouseDrag(int button, float x, float y){
    if (onMouseDragFunc != NULL){
        onMouseDragFunc(button, x, y);
    }
    if (onMouseDragLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onMouseDragLuaFunc);
        lua_pushnumber(luastate, button);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(3, 0, 0);
    }
}

void Supernova::onMouseMove(void (*onMouseMoveFunc)(float, float)){
    Supernova::onMouseMoveFunc = onMouseMoveFunc;
}

int Supernova::onMouseMove(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onMouseMoveLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onMouseMove(float x, float y){
    if (onMouseMoveFunc != NULL){
        onMouseMoveFunc(x, y);
    }
    if (onMouseMoveLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onMouseMoveLuaFunc);
        lua_pushnumber(luastate, x);
        lua_pushnumber(luastate, y);
        luaCallback(2, 0, 0);
    }
}

void Supernova::onKeyPress(void (*onKeyPressFunc)(int)){
    Supernova::onKeyPressFunc = onKeyPressFunc;
}

int Supernova::onKeyPress(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onKeyPressLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onKeyPress(int key){
    if (onKeyPressFunc != NULL){
        onKeyPressFunc(key);
    }
    if (onKeyPressLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onKeyPressLuaFunc);
        lua_pushnumber(luastate, key);
        luaCallback(1, 0, 0);
    }
}

void Supernova::onKeyUp(void (*onKeyUpFunc)(int)){
    Supernova::onKeyUpFunc = onKeyUpFunc;
}

int Supernova::onKeyUp(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        Supernova::onKeyUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Supernova::call_onKeyUp(int key){
    if (onKeyUpFunc != NULL){
        onKeyUpFunc(key);
    }
    if (onKeyUpLuaFunc != 0){
        /* your function is once again on the top of the stack! */
        lua_rawgeti(luastate, LUA_REGISTRYINDEX, Supernova::onKeyUpLuaFunc);
        lua_pushnumber(luastate, key);
        luaCallback(1, 0, 0);
    }
}
