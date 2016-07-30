#ifndef supernova_h
#define supernova_h

#define S_GLES2 1

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "Scene.h"


void init();


class Supernova {

private:
	static lua_State *luastate;
	static Scene *mainScene;

    static void (*onFrameFunc)();
    static int onFrameLuaFunc;

    static void (*onTouchPressFunc)(float, float);
    static int onTouchPressLuaFunc;

    static void (*onTouchUpFunc)(float, float);
    static int onTouchUpLuaFunc;

    static void (*onTouchDragFunc)(float, float);
    static int onTouchDragLuaFunc;

    static void (*onMousePressFunc)(int, float, float);
    static int onMousePressLuaFunc;

    static void (*onMouseUpFunc)(int, float, float);
    static int onMouseUpLuaFunc;

    static void (*onMouseDragFunc)(int, float, float);
    static int onMouseDragLuaFunc;

    static void (*onMouseMoveFunc)(float, float);
    static int onMouseMoveLuaFunc;

    static void (*onKeyPressFunc)(int);
    static int onKeyPressLuaFunc;

    static void (*onKeyUpFunc)(int);
    static int onKeyUpLuaFunc;

    static int screenWidth;
    static int screenHeight;

    static int canvasWidth;
    static int canvasHeight;

    static int preferedCanvasWidth;
    static int preferedCanvasHeight;

		static int renderAPI;
    static bool mouseAsTouch;
    static bool useDegrees;

    static void luaCallback(int nargs, int nresults, int msgh);

public:
	Supernova();
	virtual ~Supernova();

    static void setLuaState(lua_State*);
	static lua_State* getLuaState();

	static void setScene(Scene *mainScene);
	static Scene* getScene();

    static int getScreenWidth();
    static int getScreenHeight();
    static void setScreenSize(int screenWidth, int screenHeight);

    static int getCanvasWidth();
    static int getCanvasHeight();
    static void setCanvasSize(int canvasWidth, int canvasHeight);

    static int getPreferedCanvasWidth();
    static int getPreferedCanvasHeight();
    static void setPreferedCanvasSize(int preferedCanvasWidth, int preferedCanvasHeight);

		static void setRenderAPI(int renderAPI);
		static int getRenderAPI();

    static void setMouseAsTouch(bool mouseAsTouch);
    static bool isMouseAsTouch();

    static void setUseDegrees(bool useDegrees);
    static bool isUseDegrees();

    static void onFrame(void (*onFrameFunc)());
    static int onFrame(lua_State *L);
    static void call_onFrame();

    static void onTouchPress(void (*onTouchPressFunc)(float, float));
    static int onTouchPress(lua_State *L);
    static void call_onTouchPress(float x, float y);

    static void onTouchUp(void (*onTouchUpFunc)(float, float));
    static int onTouchUp(lua_State *L);
    static void call_onTouchUp(float x, float y);

    static void onTouchDrag(void (*onTouchDragFunc)(float, float));
    static int onTouchDrag(lua_State *L);
    static void call_onTouchDrag(float x, float y);

    static void onMousePress(void (*onMousePressFunc)(int, float, float));
    static int onMousePress(lua_State *L);
    static void call_onMousePress(int button, float x, float y);

    static void onMouseUp(void (*onMouseUpFunc)(int, float, float));
    static int onMouseUp(lua_State *L);
    static void call_onMouseUp(int button, float x, float y);

    static void onMouseDrag(void (*onMouseDragFunc)(int, float, float));
    static int onMouseDrag(lua_State *L);
    static void call_onMouseDrag(int button, float x, float y);

    static void onMouseMove(void (*onMouseMoveFunc)(float, float));
    static int onMouseMove(lua_State *L);
    static void call_onMouseMove(float x, float y);

    static void onKeyPress(void (*onKeyPressFunc)(int));
    static int onKeyPress(lua_State *L);
    static void call_onKeyPress(int key);

    static void onKeyUp(void (*onKeyUpFunc)(int));
    static int onKeyUp(lua_State *L);
    static void call_onKeyUp(int key);
};

#endif /* CORE_GAME_H_ */
